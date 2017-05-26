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
License: GPV2
Summary: Expose MicroChip UnicensV2 through AGL AppFw
Url:     https://github.com/iotbzh/unicens-agent

Provides: unicens-agent
Prefix: /opt/unicens-agent
BuildRequires: pkg-config , pkgconfig(libsystemd>=222), pkgconfig(libmicrohttpd>=0.9.54), pkgconfig(afb-daemon), pkgconfig(json-c), pkgconfig(mxml)

BuildRoot:/home/fulup/Workspace/AGL-AppFW/unicens2rc-afb/build

%description 
Expose MicroChip UnicensV2 through AGL AppFw

%prep

%build
(mkdir -p build; cd build; cmake ..; make)

%install
(cd build; make populate DESTDIR=%{buildroot})

