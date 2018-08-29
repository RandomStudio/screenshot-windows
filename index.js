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

const takeScreenshot = ({ filename = 'screenshot.jpg', quality = 80, width, height, fit } = {}) => {
	const screenWidth = addon.getWidth();
	const screenHeight = addon.getHeight();

	// ensure valid argument types
	filename = String(filename);
	quality = Math.round(Number(quality));
	width = Math.round(Number(width));
	height = Math.round(Number(height));
	fit = Math.round(Number(fit));

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
	const screenWidth = addon.getWidth();
	const screenHeight = addon.getHeight();
	console.log('result:', addon.takeScreenshot('image/jpeg', 'screenshot.jpg', 80, screenWidth, screenHeight));
}

module.exports = takeScreenshot;
