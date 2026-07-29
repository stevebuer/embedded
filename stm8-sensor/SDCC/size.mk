# Ensure this runs as a standalone target when called explicitly

TARGET=STM8S105/STM8S105

.PHONY: check-size

check-size:
	@if [ ! -f $(TARGET).map ]; then \
		echo "Error: $(TARGET).map not found. Run 'make' first."; \
		exit 1; \
	fi
	@echo "=================================================="
	@echo "          STM8S105 FIRMWARE SECTION SIZES         "
	@echo "=================================================="
	@echo "--- RAM SECTIONS ---"
	@awk '/^Area/ {flag=1; next} /^$$/ {flag=0} flag {print}' $(TARGET).map | \
		grep -E "DABS|DATA|INITIALIZED" || true
	@echo ""
	@echo "--- FLASH SECTIONS ---"
	@awk '/^Area/ {flag=1; next} /^$$/ {flag=0} flag {print}' $(TARGET).map | \
		grep -E "CODE|CONST|INITIALIZER" || true
	@echo "=================================================="
	@echo "Total Flash Used (approx):"
	@grep -E "CODE|CONST|INITIALIZER" $(TARGET).map | \
		grep -oE '[0-9]+\.\s+bytes' | grep -oE '[0-9]+' | \
		paste -sd+ - | bc | awk '{print $$1 " bytes"}'

