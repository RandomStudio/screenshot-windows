const addon = require('bindings')('addon.node');

const errorCodes = new Map([
	[-1, 'GDI+ cannot find the requested encoder'],
	[-2, 'Failed to save the JPEG to disk'],
	[-3, 'Failed to create a compatible memory device context'],
	[-4, 'Failed to create a compatible bitmap from the Window device context'],
	[-5, 'Failed to bit block transfer into compatible memory device context'],
]);

const unknownError = 'Result did not match with a known error code';

// FUNCTION FOR TESTING WITH COMMAND LINE ARGUMENTS

if (process.argv.includes('getWidth')) {
	console.log('width:', addon.getWidth());
}

if (process.argv.includes('getHeight')) {
	console.log('height:', addon.getHeight());
}

if (process.argv.includes('takeScreenshot')) {
	console.log('result:', addon.takeScreenshot('screenshot.jpg', 80, 1280, 720));
}

// EXPOSED FUNCTION

module.exports = () => {

};
