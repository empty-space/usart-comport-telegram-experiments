const path = require('path');

module.exports = {
    entry: './src/index.js',
    output: {
        filename: 'main.js',
        path: path.resolve(__dirname, 'static/js/'),
        libraryTarget: 'var',
        library: 'ui'
    }
};
