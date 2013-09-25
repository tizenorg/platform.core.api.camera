Name:       capi-media-camera
Summary:    A Camera library in Tizen C API
Version:    0.1.4
Release:    0
Group:      Multimedia/API
License:    Apache-2.0
Source0:    %{name}-%{version}.tar.gz
BuildRequires:  cmake
BuildRequires:  pkgconfig(dlog)
BuildRequires:  pkgconfig(mm-camcorder)
BuildRequires:  pkgconfig(audio-session-mgr)
BuildRequires:  pkgconfig(capi-base-common)

%description
A Camera library in Tizen C API.


%package devel
Summary:  A Camera library in Tizen C API (Development)
Requires: %{name} = %{version}-%{release}

%description devel
A Camera library in Tizen C API.

Development related files.



%prep
%setup -q


%build
MAJORVER=`echo %{version} | awk 'BEGIN {FS="."}{print $1}'`
%cmake . -DFULLVER=%{version} -DMAJORVER=${MAJORVER}


make %{?jobs:-j%jobs}

%install
%make_install

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig


%files
%manifest capi-media-camera.manifest
%license LICENSE.APLv2
%{_libdir}/libcapi-media-camera.so.*


%files devel
%{_includedir}/media/camera.h
%{_libdir}/pkgconfig/*.pc
%{_libdir}/libcapi-media-camera.so


