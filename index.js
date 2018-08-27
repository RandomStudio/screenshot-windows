const addon = require('bindings')('addon.node');

const errorCodes = new Map([
	[-1, 'GDI+ cannot find the JPEG encoder'],
]);

const unknownError = 'Result did not match with a valid rotation or known error code';

// FUNCTION FOR TESTING WITH COMMAND LINE ARGUMENTS

if (process.argv.includes('getWidth')) {
	console.log('width:', addon.getWidth());
}

if (process.argv.includes('getHeight')) {
	console.log('height:', addon.getHeight());
}

// EXPOSED FUNCTION

module.exports = () => {

};
