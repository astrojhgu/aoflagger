/** @file
 * This is the header file for the PngFile class.
 * @author André Offringa <offringa@gmail.com>
 */
#ifndef PNGFILE_H
#define PNGFILE_H

#include <string>

#include <png.h>

#include "../baseexception.h"

/**
 * This class wraps the libpng library. It can save an Image2D class to a .png file.
 * @see Image2D
 */
class PngFile {
	public:
		/**
		 * Construct a new png file with a filename, a width and a height.
		 * @param filename Name of the png file.
		 * @param width Width of the image
		 * @param height Height of the image
		 */
		PngFile(const std::string &filename, unsigned width, unsigned height);
		
		/**
		 * Destructor.
		 */
		~PngFile();
		
		/**
		 * Start writing.
		 * @throws IOException if something goes wrong.
		 */
		void BeginWrite();
		
		/**
		 * Closes the image.
		 */
		void Close();
		
		/**
		 * Returns the size of one pixel in bytes.
		 * @return Size of one pixel in bytes.
		 */
		int PixelSize() const throw() { return _pixelSize; }
		
		/**
		 * Clears the entire image.
		 * @param colorR Red background value.
		 * @param colorG Green background value.
		 * @param colorB Blue background value.
		 * @param colorA Alfa background value.
		 */
		void Clear(int colorR=255, int colorG=255, int colorB=255, int colorA=255);

		/**
		 * Sets a pixel in the image to a specific color.
		 * @param x x-coordinate.
		 * @param y y-coordinate.
		 * @param colorR Red value.
		 * @param colorG Green value.
		 * @param colorB Blue value.
		 * @param colorA Alfa value.
		 */
		void PlotPixel(unsigned x, unsigned y, int colorR, int colorG, int colorB, int colorA)
		{
			_row_pointers[y][x*_pixelSize] = colorR;
			_row_pointers[y][x*_pixelSize+1] = colorG;
			_row_pointers[y][x*_pixelSize+2] = colorB;
			_row_pointers[y][x*_pixelSize+3] = colorA;
		}

		/**
		 * Sets a square in the image to a specific color.
		 * @param x x-coordinate.
		 * @param y y-coordinate.
		 * @param colorR Red value.
		 * @param colorG Green value.
		 * @param colorB Blue value.
		 * @param colorA Alfa value.
		 */
		void PlotDatapoint(unsigned x, unsigned y, int colorR, int colorG, int colorB, int colorA);
		
		/**
		 * Retrieve the array of row pointers.
		 * @return an array of row pointers.
		 */
		png_bytep *RowPointers() const throw() { return _row_pointers; }
		
		/**
		 * Sets all pixels in the rowpointers to match the image.
     */
		void SetFromImage(const class Image2D &image, const class ColorMap &colorMap, long double normalizeFactor, long double zeroLevel = 0.0);

		/**
		 * Write an image directly to disk. The image will be normalized.
		 * @param image Image containing the data.
		 * @param filename Name of the file to write.
		 * @throws IOException if writing fails.
		 */
		static void Save(const class Image2D &image, const std::string &filename);
		
		/**
		 * Write an image directly to disk by using a specific colormap. The image will be normalized.
		 * @param image Image containing the data.
		 * @param filename Name of the file to write.
		 * @param colorMap ColorMap to use.
		 * @throws IOException if writing fails.
		 */
		static void Save(const class Image2D &image, const std::string &filename, const class ColorMap &colorMap);
		
		/**
		 * Write an image directly to disk by using a specific colormap. The image will be normalized with a specified
		 * factor.
		 * @param image Image containing the data.
		 * @param filename Name of the file to write.
		 * @param colorMap ColorMap to use.
		 * @param normalizeFactor Factor to use for normalisation.
		 * @throws IOException if writing fails.
		 */
		static void Save(const class Image2D &image, const std::string &filename, const class ColorMap &colorMap, long double normalizeFactor, long double zeroLevel = 0.0);
		
		/**
		 * Fill this instance with the values of the image by using the color map, and save it to disk.
		 * @param image Image to use.
		 * @param colorMap Color map to use.
		 * @throws IOException if writing fails.
		 */
		void Save(const class Image2D &image, const class ColorMap &colorMap);
	private:
		const std::string _filename;
		const unsigned _width, _height;
		png_bytep *_row_pointers;
		png_structp _png_ptr;
		png_infop _info_ptr;
		FILE *_fp;
		const int _pixelSize;
};

#endif
