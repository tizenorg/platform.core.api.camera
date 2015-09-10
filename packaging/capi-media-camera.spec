%bcond_with wayland
%bcond_with x

Name:       capi-media-camera
Summary:    A Media Daemon Camera library in Tizen C API
Version:    0.2.6
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
BuildRequires:  pkgconfig(capi-media-sound-manager)
BuildRequires:  pkgconfig(mmsvc-camera)
BuildRequires:  pkgconfig(capi-media-recorder)
BuildRequires:  pkgconfig(appcore-efl)
BuildRequires:  pkgconfig(elementary)
BuildRequires:  pkgconfig(ecore)
BuildRequires:  pkgconfig(evas)
BuildRequires:  pkgconfig(capi-media-tool)
%if %{with x}
BuildRequires:  pkgconfig(ecore-x)
%endif
%if %{with wayland}
BuildRequires:  pkgconfig(ecore-wayland)
%endif
BuildRequires:  pkgconfig(json)
BuildRequires:  pkgconfig(libtbm)
BuildRequires:  pkgconfig(eom)

Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig


%description
A Media Camera Daemon library in Tizen Native API.

%package devel
Summary:  A Media Camera Daemon library in Tizen Native API. (Development)
Group:    Development/Multimedia
Requires: %{name} = %{version}-%{release}
Requires:  pkgconfig(mm-common)
%description devel
A Camera library in Tizen C API.

Development related files.


%prep
%setup -q

%build
%if 0%{?sec_build_binary_debug_enable}
export CFLAGS="$CFLAGS -DTIZEN_DEBUG_ENABLE"
#export CFLAGS+=" -D_USE_X_DIRECT_"
export CXXFLAGS="$CXXFLAGS -DTIZEN_DEBUG_ENABLE"
export FFLAGS="$FFLAGS -DTIZEN_DEBUG_ENABLE"
%endif
MAJORVER=`echo %{version} | awk 'BEGIN {FS="."}{print $1}'`
cmake . -DCMAKE_INSTALL_PREFIX=/usr -DFULLVER=%{version} -DMAJORVER=${MAJORVER} \
%if %{with wayland}
	-DWAYLAND_SUPPORT=On \
%else
	-DWAYLAND_SUPPORT=Off \
%endif
%if %{with x}
	-DX11_SUPPORT=On \
%else
	-DX11_SUPPORT=Off \
%endif

make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
mkdir -p %{buildroot}/usr/share/license
mkdir -p %{buildroot}/opt/usr/devel
mkdir -p %{buildroot}/usr/bin
cp LICENSE.APLv2 %{buildroot}/usr/share/license/%{name}
#cp test/camera_museD_test %{buildroot}/usr/bin

%make_install

%post
/sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%manifest capi-media-camera.manifest
%{_libdir}/libcapi-media-camera.so*
%{_datadir}/license/%{name}
#/opt/usr/devel/*
#/usr/bin/camera_museD_test
#%{_bindir}/multimedia_camera_test

%files devel
%{_includedir}/media/*.h
%{_libdir}/pkgconfig/*.pc
