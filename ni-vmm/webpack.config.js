const path = require('path');

module.exports = {
    context: path.resolve(__dirname, './src/main/resources/static'),
    devtool: 'source-map',
    entry: {
        search: './js/search.js',
        index: './js/index.js',
        database: './js/database.js',
        settings: './js/settings.js',
    },
    output: {
        filename: '[name].bundle.js',
        path: path.resolve(__dirname, './target/classes/static/output'),
        clean: true
    },
    module: {
        rules: [
            {
                test: /\.js$/,
                exclude: /node_modules/,
                use: {
                    loader: 'babel-loader',
                },
            },
            {
                test: /\.css$/i,
                use: ['style-loader', 'css-loader'],
            },
            {
                test: /\.woff2?$/,
                type: "asset/resource",
            },
            {
                test: /\.s[ac]ss$/i,
                use: [
                    // Creates `style` nodes from JS strings
                    "style-loader",
                    // Translates CSS into CommonJS
                    "css-loader",
                    // Compiles Sass to CSS
                    "sass-loader",
                ],
            },
        ],
    },
};
