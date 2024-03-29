// Original file Copyright CMU462 Fall 2015:
// Kayvon Fatahalian, Keenan Crane,
// Sky Gao, Bryce Summers, Michael Choquette.
#include "svgparser.h"
#include "base64.h"
#include "lodepng.h"
#include "renderers/texture.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

using namespace std;

SVG *SVGParser::curr_svg;
string SVGParser::dir;

#include "color.hpp"

// Parser //

int SVGParser::load(const char *filename, SVG *svg)
{
	ifstream in(filename);
	if (!in.is_open()) {
		return -1;
	}
	in.close();

	dir = filename;

	XMLDocument doc;
	doc.LoadFile(filename);
	if (doc.Error()) {
		doc.PrintError();
		exit(1);
	}

	XMLElement *root = doc.FirstChildElement("svg");
	if (!root) {
		cerr << "Error: not an SVG file!" << endl;
		exit(1);
	}

	root->QueryFloatAttribute("width", &svg->width);
	root->QueryFloatAttribute("height", &svg->height);

	curr_svg = svg;
	parseSVG(root, svg);

	return 0;
}

void SVGParser::parseSVG(XMLElement *xml, SVG *svg)
{
	/* NOTE (sky):
   * SVG uses a "painters model" when drawing elements. Elements 
   * that appear later in the document are drawn after (on top of) 
   * elements that appear earlier in the document. The parser loads
   * elements in the same order and the renderer should respect this
   * order when drawing elements.
   */

	XMLElement *elem = xml->FirstChildElement();
	while (elem) {
		string elementType(elem->Value());
		if (elementType == "line") {
			Line *line = new Line();
			parseElement(elem, line);
			parseLine(elem, line);
			svg->elements.push_back(line);

		} else if (elementType == "polyline") {
			Polyline *polyline = new Polyline();
			parseElement(elem, polyline);
			parsePolyline(elem, polyline);
			svg->elements.push_back(polyline);

		} else if (elementType == "rect") {
			float w = elem->FloatAttribute("width");
			float h = elem->FloatAttribute("height");

			// treat zero-size rectangles as points
			if (w == 0 && h == 0) {
				Point *point = new Point();
				parseElement(elem, point);
				parsePoint(elem, point);
				svg->elements.push_back(point);
			} else {
				Rect *rect = new Rect();
				parseElement(elem, rect);
				parseRect(elem, rect);
				svg->elements.push_back(rect);
			}

		} else if (elementType == "polygon") {
			Polygon *polygon = new Polygon();
			parseElement(elem, polygon);
			parsePolygon(elem, polygon);
			svg->elements.push_back(polygon);

		} else if (elementType == "image") {
			SVGImage *image = new SVGImage();
			parseElement(elem, image);
			parseImage(elem, image);
			svg->elements.push_back(image);

		} else if (elementType == "g") {
			Group *group = new Group();
			parseElement(elem, group);
			parseGroup(elem, group);
			svg->elements.push_back(group);

		} else if (elementType == "colortri") {
			ColorTri *ctri = new ColorTri();
			parseElement(elem, ctri);
			parseColorTri(elem, ctri);
			svg->elements.push_back(ctri);

		} else if (elementType == "textri") {
			TexTri *ttri = new TexTri();
			parseElement(elem, ttri);
			parseTexTri(elem, ttri);
			svg->elements.push_back(ttri);

		} else if (elementType == "texture") {
			parseTexture(elem);

		} else {
			// unknown element type --- include default handler here if desired
		}

		elem = elem->NextSiblingElement();
	}
}

void SVGParser::parseElement(XMLElement *xml, SVGElement *element)
{
	// parse style
	Style *style = &element->style;
	const char *fill = xml->Attribute("fill");
	if (fill)
		style->fillColor = Color::fromHex(fill);

	//const char* fill_opacity = xml->Attribute( "fill-opacity" );
	//if( fill_opacity ) style->fillColor.a = atof( fill_opacity );

	const char *stroke = xml->Attribute("stroke");
	//const char* stroke_opacity = xml->Attribute( "stroke-opacity" );
	if (stroke) {
		style->strokeColor = Color::fromHex(stroke);
		style->strokeVisible = true;
		//if( stroke_opacity ) style->strokeColor.a = atof( stroke_opacity );
	} else {
		style->strokeColor = Color::Black;
		style->strokeVisible = false;
		//style->strokeColor.a = 0;
	}

	xml->QueryFloatAttribute("stroke-width", &style->strokeWidth);
	xml->QueryFloatAttribute("stroke-miterlimit", &style->miterLimit);

	// parse transformation
	const char *trans = xml->Attribute("transform");
	if (trans) {
		// NOTE (sky):
		// This implements the SVG transformation specification. All the SVG
		// transformations are supported as documented in the link below:
		// https://developer.mozilla.org/en-US/docs/Web/SVG/Attribute/transform

		// consolidate transformation
		mat3 transform;

		string trans_str = trans;
		size_t paren_l, paren_r;
		while (trans_str.find_first_of('(') != string::npos) {
			paren_l = trans_str.find_first_of('(');
			paren_r = trans_str.find_first_of(')');

			string type = trans_str.substr(0, paren_l);
			string data = trans_str.substr(paren_l + 1, paren_r - paren_l - 1);

			mat3 m = mat3(1.0);

			if (type == "matrix") {
				string matrix_str = data;
				replace(matrix_str.begin(), matrix_str.end(), ',', ' ');

				stringstream ss(matrix_str);
				float a, b, c, d, e, f;
				ss >> a >> b >> c >> d >> e >> f;

				m = mat3(a, c, e, b, d, f, 0, 0, 1);

			} else if (type == "translate") {
				stringstream ss(data);
				float x;
				if (!(ss >> x))
					x = 0;
				float y;
				if (!(ss >> y))
					y = 0;

				m = translate(m, vec2(x, y));

			} else if (type == "scale") {
				stringstream ss(data);
				float x;
				if (!(ss >> x))
					x = 1;
				float y;
				if (!(ss >> y))
					y = 1;

				m = scale(m, vec2(x, y));

			} else if (type == "rotate") {
				stringstream ss(data);
				float a;
				if (!(ss >> a))
					a = 0;
				float x;
				if (!(ss >> x))
					x = 0;
				float y;
				if (!(ss >> y))
					y = 0;

				m = translate(m, vec2(-x, -y));
				m = rotate(m, a);
				m = translate(m, vec2(x, y));

			} else if (type == "skewX") {
				stringstream ss(data);
				float a;
				ss >> a;

				m[0].y = tan(radians(a));

			} else if (type == "skewY") {
				stringstream ss(data);
				float a;
				ss >> a;

				m[1].x = tan(radians(a));

			} else {
				cerr << "unknown transformation type: " << type << endl;
			}

			transform = transform * m;
			size_t end = paren_r + 2;
			trans_str.erase(0, end);
		}

		element->transform = transform;
	}
}

void SVGParser::parseTexture(XMLElement *xml)
{
	string texid = xml->Attribute("texid");

	const char *file = xml->Attribute("filename");
	vector<unsigned char> pixels;
	unsigned int width, height;
	int err = lodepng::decode(pixels, width, height, dir + string(file));
	if (err) {
		cerr << " could not load image " << file << endl;
		return;
	}

	// Strip the alpha channel
	vector<unsigned char> pixels_no_alpha;
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			for (int k = 0; k < 3; ++k) {
				pixels_no_alpha.push_back(pixels[4 * (y * width + x) + k]);
			}
		}
	}

	Texture *tex = new Texture();
	tex->init(pixels_no_alpha, width, height);
	curr_svg->textures[texid] = tex;
}

void SVGParser::parsePoint(XMLElement *xml, Point *point)
{
	point->position = vec2(xml->FloatAttribute("x"), xml->FloatAttribute("y"));
}

void SVGParser::parseLine(XMLElement *xml, Line *line)
{
	line->from = vec2(xml->FloatAttribute("x1"), xml->FloatAttribute("y1"));
	line->to = vec2(xml->FloatAttribute("x2"), xml->FloatAttribute("y2"));
}

void SVGParser::parsePolyline(XMLElement *xml, Polyline *polyline)
{
	stringstream points(xml->Attribute("points"));

	float x, y;
	char c;

	while (points >> x >> c >> y) {
		polyline->points.push_back(vec2(x, y));
	}
}

void SVGParser::parseRect(XMLElement *xml, Rect *rect)
{
	rect->position = vec2(xml->FloatAttribute("x"), xml->FloatAttribute("y"));
	rect->dimension =
		vec2(xml->FloatAttribute("width"), xml->FloatAttribute("height"));
}

void SVGParser::parsePolygon(XMLElement *xml, Polygon *polygon)
{
	stringstream points(xml->Attribute("points"));

	float x, y;
	char c;

	while (points >> x >> c >> y) {
		polygon->points.push_back(vec2(x, y));
	}
}

void SVGParser::parseImage(XMLElement *xml, SVGImage *image)
{
	image->position = vec2(xml->FloatAttribute("x"), xml->FloatAttribute("y"));
	image->dimension =
		vec2(xml->FloatAttribute("width"), xml->FloatAttribute("height"));

	// read png data
	const char *data = xml->Attribute("xlink:href");
	while (*data != ',')
		data++;
	data++;

	// decode base64 encoded data
	string encoded = data;
	encoded.erase(remove(encoded.begin(), encoded.end(), ' '), encoded.end());
	encoded.erase(remove(encoded.begin(), encoded.end(), '\t'), encoded.end());
	encoded.erase(remove(encoded.begin(), encoded.end(), '\n'), encoded.end());
	string decoded = base64_decode(encoded);

	// load decoded data into buffer
	const unsigned char *buffer = (unsigned char *)decoded.c_str();
	size_t size = decoded.size();

	vector<unsigned char> pixels;
	unsigned int width, height;
	int err = lodepng::decode(pixels, width, height, buffer, size);
	if (err) {
		cerr << " could not load image " << endl;
		return;
	}
	// load into png
	// PNG png; PNGParser::load(buffer, size, png);

	// Strip the alpha channel
	vector<unsigned char> pixels_no_alpha;
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			for (int k = 0; k < 3; ++k) {
				pixels_no_alpha.push_back(pixels[4 * (y * width + x) + k]);
			}
		}
	}

	// create bitmap texture from png (mip level 0)
	MipLevel mip_start;
	mip_start.width = width;
	mip_start.height = height;
	mip_start.texels = pixels_no_alpha;

	// add to svg
	image->tex.width = mip_start.width;
	image->tex.height = mip_start.height;
	image->tex.mipmap.push_back(mip_start);
}

void SVGParser::parseGroup(XMLElement *xml, Group *group)
{
	/* NOTE (sky):
   * A group contains a list of elements, and optionally a transformation
   * to apply to all the elements it contains. Elements in a group follow
   * the same draw order as elements in a svg (top to bottom).  
   * A group should be considered as one single element outside its scope.
   * This means at draw time, all elements in a group should be drawn before 
   * elements outside the group. All elements in the group inherits the group
   * transformation, and keep in mind that transformation is accumulative.
   * Groups can also be nested.  
   */
	XMLElement *elem = xml->FirstChildElement();
	while (elem) {
		string elementType(elem->Value());
		if (elementType == "line") {
			Line *line = new Line();
			parseElement(elem, line);
			parseLine(elem, line);
			group->elements.push_back(line);

		} else if (elementType == "polyline") {
			Polyline *polyline = new Polyline();
			parseElement(elem, polyline);
			parsePolyline(elem, polyline);
			group->elements.push_back(polyline);

		} else if (elementType == "rect") {
			float w = elem->FloatAttribute("width");
			float h = elem->FloatAttribute("height");

			// treat zero-size rectangles as points
			if (w == 0 && h == 0) {
				Point *point = new Point();
				parseElement(elem, point);
				parsePoint(elem, point);
				group->elements.push_back(point);
			} else {
				Rect *rect = new Rect();
				parseElement(elem, rect);
				parseRect(elem, rect);
				group->elements.push_back(rect);
			}

		} else if (elementType == "polygon") {
			Polygon *polygon = new Polygon();
			parseElement(elem, polygon);
			parsePolygon(elem, polygon);
			group->elements.push_back(polygon);

		} else if (elementType == "image") {
			SVGImage *image = new SVGImage();
			parseElement(elem, image);
			parseImage(elem, image);
			group->elements.push_back(image);

		} else if (elementType == "g") {
			Group *sub_group = new Group();
			parseElement(elem, sub_group);
			parseGroup(elem, sub_group);
			group->elements.push_back(sub_group);

		} else if (elementType == "colortri") {
			ColorTri *ctri = new ColorTri();
			parseElement(elem, ctri);
			parseColorTri(elem, ctri);
			group->elements.push_back(ctri);

		} else if (elementType == "textri") {
			TexTri *ttri = new TexTri();
			parseElement(elem, ttri);
			parseTexTri(elem, ttri);
			group->elements.push_back(ttri);

		} else if (elementType == "texture") {
			parseTexture(elem);

		} else {
			// unknown element type --- include default handler here if desired
		}
		elem = elem->NextSiblingElement();
	}
}

void SVGParser::parseColorTri(XMLElement *xml, ColorTri *ctri)
{
	stringstream points(xml->Attribute("points"));

	float x, y;
	points >> x >> y;
	ctri->p0_svg = vec2(x, y);
	points >> x >> y;
	ctri->p1_svg = vec2(x, y);
	points >> x >> y;
	ctri->p2_svg = vec2(x, y);

	stringstream colors(xml->Attribute("colors"));

	float r, g, b, a;
	// Alpha removed
	colors >> r >> g >> b >> a;
	ctri->p0_col = vec3(r, g, b);
	colors >> r >> g >> b >> a;
	ctri->p1_col = vec3(r, g, b);
	colors >> r >> g >> b >> a;
	ctri->p2_col = vec3(r, g, b);
}

void SVGParser::parseTexTri(XMLElement *xml, TexTri *ttri)
{
	stringstream points(xml->Attribute("points"));

	float x, y;
	points >> x >> y;
	ttri->p0_svg = vec2(x, y);
	points >> x >> y;
	ttri->p1_svg = vec2(x, y);
	points >> x >> y;
	ttri->p2_svg = vec2(x, y);

	stringstream uvs(xml->Attribute("uvs"));

	uvs >> x >> y;
	ttri->p0_uv = vec2(x, y);
	uvs >> x >> y;
	ttri->p1_uv = vec2(x, y);
	uvs >> x >> y;
	ttri->p2_uv = vec2(x, y);

	// read png data
	string texid = xml->Attribute("texid");

	ttri->tex = curr_svg->textures[texid];
}
