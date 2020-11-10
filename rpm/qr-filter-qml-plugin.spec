Name:       qr-filter-qml-plugin
Summary:    QR code qml wrappper based on QZXing
Version:    1.0
Release:    1
Group:      System/Libraries
License:    Apache License 2.0
URL:        https://git.omprussia.ru/os/qr-filter-qml-plugin
Source:     %{name}-%{version}.tar.bz2
BuildRequires: pkgconfig(Qt5Core)
BuildRequires: pkgconfig(Qt5Quick)
BuildRequires: pkgconfig(Qt5Multimedia)
BuildRequires: pkgconfig(Qt5Concurrent)
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
%{_libdir}/qt5/qml/ru/omprussia/qrfilter/*
