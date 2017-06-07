###########################################################################
# Copyright 2015, 2016, 2017 IoT.bzh
#
# author: Fulup Ar Foll <fulup@iot.bzh>
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
###########################################################################


Name:    unicens-agent
Version: 0.1
Release: 1
License: Apache-V2
Summary: Expose MicroChip UnicensV2 through AGL AppFw
Url:     https://github.com/iotbzh/unicens-agent
Source0: %{name}_%{version}.orig.tar.gz

Prefix: /opt/unicens-agent
BuildRequires: cmake
BuildRequires: gcc gcc-c++
BuildRequires: pkgconfig(libsystemd) >= 222,
BuildRequires: pkgconfig(libmicrohttpd) >= 0.9.54
BuildRequires: pkgconfig(afb-daemon), pkgconfig(json-c), pkgconfig(mxml)

BuildRoot:%{_tmppath}/%{name}-%{version}-build

%description 
Expose MicroChip UnicensV2 through AGL AppFw

%prep
%setup -q

%build
%cmake -DBINDINGS_INSTALL_PREFIX:PATH=%{_libdir}
%__make %{?_smp_mflags}

%install
[ -d build ] && cd build
%make_install

%files
%defattr(-,root,root)
%dir %{_libdir}/unicens-agent
%{_libdir}/unicens-agent/afb-ucs2.so
