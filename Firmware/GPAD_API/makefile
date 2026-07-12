# Copyright 2025, Robert Read
#
# This program includes free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of the
# License, or (at your option) any later version.
#
# See the GNU Affero General Public License for more details.
# You should have received a copy of the GNU Affero General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
#

PIO ?= pio

.PHONY: build run upload monitor build-fs uploadfs

build:
	$(PIO) run

run: upload
	$(PIO) device monitor -b 115200

upload:
	$(PIO) run -t upload

monitor:
	$(PIO) device monitor -b 115200

build-fs:
	$(PIO) run --target buildfs

uploadfs:
	$(PIO) run --target uploadfs
