void prepareSingleOutputColumn(char baseColumn)
{
	uint8_t outputColumnCurrentPixel = 0;
	
	for(uint8_t basePixelCounter = 0; basePixelCounter < FONT_BASE_HEIGHT; basePixelCounter++) {
		bool isPixelSet = baseColumn & (1<<basePixelCounter);
		
		for(uint8_t repeatBasePixelCounter = 0; repeatBasePixelCounter < fontMultiplier; repeatBasePixelCounter++) {
			uint8_t outputColumnIndex = outputColumnCurrentPixel/8;
			uint8_t mask = (1 << (outputColumnCurrentPixel%8));
			if(isPixelSet) {
				singleOutputColumn[outputColumnIndex] |= mask;
			} else {
				singleOutputColumn[outputColumnIndex] &= ~mask;
			}
			outputColumnCurrentPixel++;
		}
	}

}

void sendFontColumnToBuffer(uint8_t x)
{
	for(int columnsCounter = 0; columnsCounter < fontMultiplier; columnsCounter++) {
		for(int columnIndex = 0; columnIndex < fontMultiplier; columnIndex++) {
			setOctet(singleOutputColumn[columnIndex], x, columnIndex*8);
			x++;
		}
	}
}