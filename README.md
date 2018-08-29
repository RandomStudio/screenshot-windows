# Screenshot Windows

Utility for taking screenshots on Windows.

## Usage

```
const screenshotWindows = require('screenshot-windows');
const result = screenshotWindows(options);
```
## Options

- `filename ('screenshot.jpg')` Absolute or relative path to save the screenshot. Supports .jpg, .jpeg or .png file extension.
- `quality (80)` JPEG encoding quality.
- `width` Width of the saved screenshot. Is automatically determined if `height` or `maxSize` is supplied, otherwise defaults to the screen width.
- `height` Height of the saved screenshot. Is automatically determined if `width` or `maxSize` is supplied, otherwise defaults to the screen height.
- `maxSize` The maximum width or height of the screenshot. Can be used without `width` or `height` to fit the screenshot in a bounding box.

## Return value

An object containing all the settings applied for taking the screenshot, including an absolute path to the screenshot file.

```
{
  filename: 'screenshot.jpg',
  filenameAbsolute: 'C:\\Users\\Random\\screenshot-windows\\screenshot.jpg',
  quality: 80,
  width: 2560,
  height: 1440
}
```
