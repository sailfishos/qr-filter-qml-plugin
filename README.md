# qr-filter-qml-plugin

**qr-filter-qml-plugin** allows you to work with QR codes via QML. Based on [zxing-cpp](https://github.com/nu-book/zxing-cpp). Currently only QR-code reading is availaible.

## How to use
Plugin based on `VideoOutput` qml-feature - filters.
```qml
import Sailfish.QrFilter 1.0

VideoOutput {
    id: videoOutput
    filters: [ qrFilter ]
}

QrFilter {
    id: qrFilter
}
```

`QrFilter` is inherited from `QAbstractVideoFilter`, so all properties from `QAbstractVideoFilter`, e.g. `active` state can be used.

```qml
QrFilter {
    id: qrFilter
    active: camera.mode == "image" // enable filter only on image mode
}
```
Additionally `QrFilter` provide `result` property, where reading result is stored and explicit invokable method for clearing result `clearResult`.

```qml
property string passResult

VideoOutput {
    id: videoOutput
    filters: [ qrFilter ]
}

QrFilter {
    id: qrFilter
}

Button {
    visible: qrFilter.result.length !== 0 ? 1.0 : 0.0
    onClicked: {
        passResult = qrFilter.result
        qrFilter.clearResult()
    }
}
```