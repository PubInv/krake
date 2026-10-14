# ── Configuration ─────────────────────────────────────────────────────────────
SHELL   := /bin/bash
PORT    ?= /dev/ttyUSB0
LOG_DIR := .pio/test_logs

# ── Colors ────────────────────────────────────────────────────────────────────
RED    := \033[0;31m
GREEN  := \033[0;32m
YELLOW := \033[0;33m
CYAN   := \033[0;36m
BOLD   := \033[1m
RESET  := \033[0m

# ── Help (default target) ─────────────────────────────────────────────────────
.PHONY: help
help:
	@echo -e "$(BOLD)Usage: make <target>$(RESET)"
	@echo ""
	@echo -e "  $(CYAN)Build:$(RESET)"
	@echo "    build           Build firmware for esp32dev"
	@echo "    clean           Clean all build artifacts"
	@echo ""
	@echo -e "  $(CYAN)Flash & Monitor:$(RESET)"
	@echo "    flash           Flash firmware to esp32dev"
	@echo "    monitor         Open serial monitor"
	@echo "    flash-monitor   Flash then open serial monitor"
	@echo ""
	@echo -e "  $(CYAN)Test:$(RESET)"
	@echo "    test            Run all native tests"
	@echo "    test-device     Run only test_device suite"
	@echo "    test-utils      Run only test_utils suite"
	@echo "    test-verbose    Run all native tests with verbose output"
	@echo ""
	@echo -e "  $(CYAN)Options:$(RESET)"
	@echo "    PORT=/dev/ttyUSB0  Override serial port (default: /dev/ttyUSB0)"

# ── Helpers ───────────────────────────────────────────────────────────────────
define run_test
	@mkdir -p $(LOG_DIR)
	@pio test $(1) 2>&1 | \
		tee $(LOG_DIR)/last.log | \
		sed 's/\[PASSED\]/\x1b[0;32m[PASSED]\x1b[0m/g; s/\[FAILED\]/\x1b[0;31m[FAILED]\x1b[0m/g; s/\bFAIL\b/\x1b[0;31mFAIL\x1b[0m/g; s/\bPASSED\b/\x1b[0;32mPASSED\x1b[0m/g'; \
	EXIT=$${PIPESTATUS[0]}; \
	echo ""; \
	if grep -q ":FAIL" $(LOG_DIR)/last.log; then \
		echo -e "$(RED)$(BOLD)============================================$(RESET)"; \
		echo -e "$(RED)$(BOLD)              FAILED TESTS                  $(RESET)"; \
		echo -e "$(RED)$(BOLD)============================================$(RESET)"; \
		echo ""; \
		grep ":FAIL" $(LOG_DIR)/last.log | while IFS=: read -r file line test msg; do \
			echo -e "  $(YELLOW)File  :$(RESET) $$file"; \
			echo -e "  $(YELLOW)Line  :$(RESET) $$line"; \
			echo -e "  $(YELLOW)Test  :$(RESET) $$test"; \
			echo -e "  $(RED)Reason:$(RESET) $$msg"; \
			echo -e "  $(RED)--------------------------------------------$(RESET)"; \
		done; \
	else \
		echo -e "$(GREEN)$(BOLD)============================================$(RESET)"; \
		echo -e "$(GREEN)$(BOLD)           All tests passed!                $(RESET)"; \
		echo -e "$(GREEN)$(BOLD)============================================$(RESET)"; \
	fi; \
	exit $$EXIT
endef
# ── Build ─────────────────────────────────────────────────────────────────────
.PHONY: build
build:
	@echo -e "$(CYAN)$(BOLD)Building firmware...$(RESET)"
	pio run -e esp32dev

.PHONY: clean
clean:
	@echo -e "$(YELLOW)$(BOLD)Cleaning build artifacts...$(RESET)"
	pio run --target clean
	@rm -rf $(LOG_DIR)

# ── Flash & Monitor ───────────────────────────────────────────────────────────
.PHONY: flash
flash:
	@echo -e "$(CYAN)$(BOLD)Flashing firmware to $(PORT)...$(RESET)"
	pio run -e esp32dev --target upload --upload-port $(PORT)

.PHONY: monitor
monitor:
	@echo -e "$(CYAN)$(BOLD)Opening serial monitor on $(PORT)...$(RESET)"
	pio device monitor --port $(PORT)

.PHONY: flash-monitor
flash-monitor: flash monitor

# ── Test ──────────────────────────────────────────────────────────────────────
.PHONY: test
test:
	$(call run_test, -e native_device -e native_utils)

.PHONY: test-device
test-device:
	$(call run_test, -e native_device)

.PHONY: test-utils
test-utils:
	$(call run_test, -e native_utils)

.PHONY: test-verbose
test-verbose:
	$(call run_test, -e native_device -e native_utils -vvv)