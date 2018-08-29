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

const takeScreenshot = ({ filename = 'screenshot.jpg', quality, width, height, fit } = {}) => {

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

	// restrict quality between 0 and 100, default to 80
	quality = quality ? Math.max(0, Math.min(100, quality)) : 80;
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
