%bcond_with wayland
%bcond_with x

Name:       capi-media-camera
Summary:    A Camera API
Version:    0.2.21
Release:    0
Group:      Multimedia/API
License:    Apache-2.0
Source0:    %{name}-%{version}.tar.gz
BuildRequires:  cmake
BuildRequires:  pkgconfig(dlog)
BuildRequires:  pkgconfig(glib-2.0)
BuildRequires:  pkgconfig(mused)
BuildRequires:  pkgconfig(mm-common)
BuildRequires:  pkgconfig(capi-base-common)
BuildRequires:  pkgconfig(mmsvc-camera)
BuildRequires:  pkgconfig(appcore-efl)
BuildRequires:  pkgconfig(elementary)
BuildRequires:  pkgconfig(ecore)
BuildRequires:  pkgconfig(evas)
BuildRequires:  pkgconfig(capi-media-tool)
BuildRequires:  pkgconfig(mm-camcorder)
BuildRequires:  pkgconfig(gstreamer-1.0)
%if %{with wayland}
BuildRequires:  pkgconfig(ecore-wayland)
%endif
BuildRequires:  pkgconfig(libtbm)

Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig



%description
A Camera library in Tizen Native API.


%package devel
Summary:  A Camera API (Development)
Requires: %{name} = %{version}-%{release}
Requires: pkgconfig(libtbm)
Requires: pkgconfig(capi-media-tool)


%description devel
Development related files for a Camera library in Tizen Native API.


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
%{_bindir}/*

%files devel
%{_includedir}/media/camera.h
%{_libdir}/pkgconfig/*.pc
%{_libdir}/libcapi-media-camera.so
