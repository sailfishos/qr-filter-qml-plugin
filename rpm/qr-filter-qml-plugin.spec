Name:       qr-filter-qml-plugin
Summary:    QML wrapper for some ZXing-cpp barcode and QR-code features
Version:    1.2
Release:    1
License:    ASL 2.0
URL:        https://github.com/sailfishos/qr-filter-qml-plugin
Source:     %{name}-%{version}.tar.bz2
BuildRequires: pkgconfig(Qt5Core)
BuildRequires: pkgconfig(Qt5Quick)
BuildRequires: pkgconfig(Qt5Multimedia)
BuildRequires: pkgconfig(Qt5DBus)
BuildRequires: pkgconfig(zxing) >= 3.0.0
BuildRequires: qt5-qttools
BuildRequires: qt5-qttools-linguist

%description
QML wrapper for XZing-cpp providing a few features:
- QrFilter: QR-code reading filter for video streams
- Barcode: Image component for rendering barcodes and QR-codes

%package ts-devel
Summary:   Translation source for qr-filter-qml-plugin
Requires:  %{name} = %{version}

%description ts-devel
%{summary}.

%prep
%setup -q

%build
%qmake5
%make_build

%install
%qmake5_install

%post -p /sbin/ldconfig
%postun -p /sbin/ldconfig

%files
%license LICENSE
%{_datadir}/translations/%{name}*_eng_en.qm
%{_libexecdir}/zxing-daemon
%{_datadir}/dbus-1/services/*.service
%{_libdir}/qt5/qml/Amber/Barcode
%{_libdir}/qt5/qml/Amber/QrFilter

%files ts-devel
%{_datadir}/translations/source/%{name}*.ts
