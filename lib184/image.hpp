#pragma once

template <class T> class Image {
    private:
	std::vector<T> buffer;
	size_t xs, ys;
	T none;

    public:
	size_t xsize()
	{
		return xs;
	}
	size_t ysize()
	{
		return ys;
	}

	T get(size_t x, size_t y)
	{
		if (x < 0 || x > xs || y < 0 || y > ys) {
			return none;
		}
		return buffer[y * xs + x];
	}
	void set(size_t x, size_t y, T value)
	{
		if (x < 0 || x > xs || y < 0 || y > ys) {
			return;
		}
		buffer[y * xs + x] = value;
	}

	T *data()
	{
		return buffer.data();
	}

	Image(size_t xs, size_t ys, T none)
	{
		this->xs = xs;
		this->ys = ys;
		this->none = none;
		buffer.resize(xs * ys);
		std::fill(buffer.begin(), buffer.end(), none);
	}
};
