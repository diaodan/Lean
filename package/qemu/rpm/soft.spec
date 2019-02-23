%define version __PACK_VERSION__
%define release __PACK_RELEASE__
%define kernelarch __KERNEL_ARCH__
%define packagerequires __PACKAGE_REQUIRES__
%define builddate __BUILD_DATE__


Name: __PACK_NAME__
Version: %{version}
Release: %{release}
Summary: __PACK_COMPANY__ __PACK_NAME__ Tool

Group: Utilites/System
License: Commercial
URL: http://__PACK_URL__
Source: %{name}-%{version}.tar.gz

Requires: %{packagerequires}

%description
 -===============================================================-
  This package contains the __PACK_COMPANY__ __PACK_NAME__ tools.
  Version:          %{version}
  Build Date:       %{builddate}
  Platform:         %{kernelarch}
 -===============================================================-


%prep
%setup -c

%build

