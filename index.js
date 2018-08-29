const path = require('path');
const addon = require('bindings')('addon.node');

const errorCodes = new Map([
	[-1, 'GDI+ cannot find the requested encoder'],
	[-2, 'Failed to save the JPEG to disk'],
	[-3, 'Failed to create a compatible memory device context'],
	[-4, 'Failed to create a compatible bitmap from the Window device context'],
	[-5, 'Failed to bit block transfer into compatible memory device context'],
]);

const unknownError = 'Result did not match with a known error code';

const takeScreenshot = ({ filename = 'screenshot.jpg', quality, width, height, maxSize } = {}) => {

	// Ensure a valid filename
	if (typeof filename !== 'string') {
		throw new Error('"filename" must be a string');
	}

	// Determine the absolute filename
	const filenameAbsolute = path.isAbsolute(filename) ? filename : path.resolve(process.cwd(), filename);

	// Determine the image encoder
	let encoder;
	const extension = path.extname(filename);
	switch (extension.toLowerCase()) {
		case '.jpg':
		case '.jpeg':
			encoder = 'image/jpeg';
			break;
		case '.png':
			encoder = 'image/png';
			break;
		default: throw new Error(`Unsupported file extension: ${extension}`);
	}

	// Ensure valid quality, restrict between 0 and 100, default to 80
	quality = Math.round(Number(quality));
	quality = quality ? Math.max(0, Math.min(100, quality)) : 80;

	// Ensure valid integer arguments
	width = Math.round(Number(width));
	height = Math.round(Number(height));
	maxSize = Math.round(Number(maxSize));

	// Get screen dimensions
	const screenWidth = addon.getWidth();
	const screenHeight = addon.getHeight();
	const aspectRatio = screenWidth / screenHeight;

	// Auto-size width or height
	if (width && !height) {
		height = Math.round(width / aspectRatio);
	}
	if (height && !width) {
		width = Math.round(height * aspectRatio);
	}

	// Restrict dimensions by max size
	if (maxSize) {
		if (aspectRatio > 1) {
			// Use width to determine dimensions
			width = width ? Math.min(width, maxSize) : maxSize;
			height = Math.round(width / aspectRatio);
		} else {
			// Use height to determine dimensions
			height = height ? Math.min(height, maxSize) : maxSize;
			width = Math.round(height * aspectRatio);
		}
	}

	// Use screen dimensions if not resolved
	if (!width) width = screenWidth;
	if (!height) height = screenHeight;

	const result = addon.takeScreenshot(encoder, filenameAbsolute, quality, width, height);
	if (result !== 0) {
		throw(new Error(errorCodes.get(result) || unknownError));
	}
	return {
		filename,
		filenameAbsolute,
		quality,
		width,
		height
	};
};

// FUNCTION FOR TESTING WITH COMMAND LINE ARGUMENTS

if (process.argv.includes('getWidth')) {
	console.log('width:', addon.getWidth());
}

if (process.argv.includes('getHeight')) {
	console.log('height:', addon.getHeight());
}

if (process.argv.includes('takeScreenshot')) {
	console.log('result:', takeScreenshot());
}

module.exports = takeScreenshot;
