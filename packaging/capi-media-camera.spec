%bcond_with wayland
%bcond_with x

Name:       capi-media-camera
Summary:    A Camera library in Tizen C API
Version:    0.2.5
Release:    0
Group:      Multimedia/API
License:    Apache-2.0
Source0:    %{name}-%{version}.tar.gz
BuildRequires:  cmake
BuildRequires:  pkgconfig(gstreamer-1.0)
BuildRequires:  pkgconfig(dlog)
BuildRequires:  pkgconfig(mm-camcorder)
BuildRequires:  pkgconfig(audio-session-mgr)
BuildRequires:  pkgconfig(capi-base-common)
BuildRequires:  pkgconfig(capi-media-tool)
BuildRequires:  pkgconfig(libtbm)
BuildRequires:  pkgconfig(evas)
BuildRequires:  pkgconfig(ecore)
BuildRequires:  pkgconfig(elementary)
BuildRequires:  pkgconfig(vconf)
Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig


%description
A Camera library in Tizen C API.


%package devel
Summary:  A Camera library in Tizen C API (Development)
Requires: %{name} = %{version}-%{release}
Requires: pkgconfig(libtbm)
Requires: pkgconfig(capi-media-tool)


%description devel
A Camera library in Tizen C API.

Development related files.


%prep
%setup -q


%build
%if 0%{?sec_build_binary_debug_enable}
export CFLAGS+=" -DTIZEN_DEBUG_ENABLE"
%endif
MAJORVER=`echo %{version} | awk 'BEGIN {FS="."}{print $1}'`
%cmake . -DCMAKE_INSTALL_PREFIX=%{_prefix} -DFULLVER=%{version} -DMAJORVER=${MAJORVER} \
%if %{with wayland}
	-DWAYLAND_SUPPORT=On \
%else
	-DWAYLAND_SUPPORT=Off \
%endif
%if %{with x}
	-DX11_SUPPORT=On
%else
	-DX11_SUPPORT=Off
%endif

make %{?jobs:-j%jobs}


%install
rm -rf %{buildroot}
%make_install
mkdir -p %{buildroot}%{_datadir}/license
cp LICENSE.APLv2 %{buildroot}%{_datadir}/license/%{name}


%post -p /sbin/ldconfig


%postun -p /sbin/ldconfig

%files
%manifest capi-media-camera.manifest
%{_libdir}/libcapi-media-camera.so.*
%{_datadir}/license/%{name}

%files devel
%{_includedir}/media/camera.h
%{_includedir}/media/camera_internal.h
%{_libdir}/pkgconfig/*.pc
%{_libdir}/libcapi-media-camera.so
