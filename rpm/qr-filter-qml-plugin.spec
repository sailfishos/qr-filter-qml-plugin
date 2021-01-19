Name:       qr-filter-qml-plugin
Summary:    QR code qml wrappper based on ZXing
Version:    1.0
Release:    1
License:    ASL 2.0
URL:        https://github.com/sailfishos/qr-filter-qml-plugin
Source:     %{name}-%{version}.tar.bz2
BuildRequires: pkgconfig(Qt5Core)
BuildRequires: pkgconfig(Qt5Quick)
BuildRequires: pkgconfig(Qt5Multimedia)
BuildRequires: pkgconfig(zxing) >= 1.1.0

%description
QML zxing wrapper with filter for QR-code reading from video stream.

%prep
%setup -q

%build
%qmake5
%make_build

%install
%make_install

%post -p /sbin/ldconfig
%postun -p /sbin/ldconfig

%files
%defattr(-,root,root,-)
%{_libexecdir}/zxing-daemon
%{_datadir}/dbus-1/services/*.service
%{_libdir}/qt5/qml/Sailfish/QrFilter/*
