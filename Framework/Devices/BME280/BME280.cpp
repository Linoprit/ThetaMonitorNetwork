#include <Config/config.h>
#include <Devices/BME280/BME280.h>

namespace boschBme280 {

uint8_t _temperature_res, _pressure_oversampling, _huminidity_oversampling,  _mode, h1, h3;
int8_t h6;
int16_t t2, t3, p2, p3, p4, p5, p6, p7, p8, p9, h2, h4, h5;
uint16_t t1, p1;
int32_t t_fine;


void Bme280::delay_us(uint16_t us)
{
	BME280_TIMER->Instance->CNT = 0;
	while( BME280_TIMER->Instance->CNT <= us ){};
}

//
//	Functions

uint8_t Bme280::read8(uint8_t addr)
{
  uint8_t tmp = 0;

  HAL_I2C_Mem_Read(&BME280_HI2C, BME280_I2CADDR, addr, 1, &tmp, 1, 10);

  return tmp;
}

uint16_t Bme280::read16(uint8_t addr)
{

	uint8_t tmp[2];

	HAL_I2C_Mem_Read(&BME280_HI2C, BME280_I2CADDR, addr, 1, tmp, 2, 10);

	return ((tmp[0] << 8) | tmp[1]);
}

uint16_t Bme280::read16LE(uint8_t addr)
{
	uint16_t tmp;

	tmp = read16(addr);
	return (tmp >> 8) | (tmp << 8);
}

void Bme280::write8(uint8_t address, uint8_t data)
{
	HAL_I2C_Mem_Write(&BME280_HI2C, BME280_I2CADDR, address, 1, &data, 1, 10);
}

uint32_t Bme280::read24(uint8_t addr)
{
	uint8_t tmp[3];

	HAL_I2C_Mem_Read(&BME280_HI2C, BME280_I2CADDR, addr, 1, tmp, 3, 10);

	return ((tmp[0] << 16) | tmp[1] << 8 | tmp[2]);
}

uint8_t Bme280::isReadingCalibration(void)
{
	uint8_t Status = read8(STATUS);

	return ((Status & 1) != 0);
}

void Bme280::setConfig(uint8_t standby_time, uint8_t filter)
{
	write8(CONFIG, (uint8_t)(((standby_time & 0x7) << 5) | ((filter & 0x7) << 2)) & 0xFC);
}

void Bme280::init(I2C_HandleTypeDef *i2c_handler, uint8_t temperature_resolution, uint8_t pressure_oversampling, uint8_t huminidity_oversampling, uint8_t mode)
{
	HAL_TIM_Base_Start(BME280_TIMER); // Timer für die Funktion Delay_us () starten

	uint8_t HumReg, i;

	if (mode > NORMALMODE)
	    mode = NORMALMODE;
	_mode = mode;
	if(mode == FORCEDMODE)
		mode = SLEEPMODE;

	if (temperature_resolution > TEMPERATURE_20BIT)
		temperature_resolution = TEMPERATURE_20BIT;
	_temperature_res = temperature_resolution;

	if (pressure_oversampling > PRESSURE_ULTRAHIGHRES)
		pressure_oversampling = PRESSURE_ULTRAHIGHRES;
	_pressure_oversampling = pressure_oversampling;

	if (huminidity_oversampling > HUMINIDITY_ULTRAHIGH)
		huminidity_oversampling = HUMINIDITY_ULTRAHIGH;
	_huminidity_oversampling = huminidity_oversampling;

	while(read8(CHIPID) != 0x60);

	write8(SOFTRESET, 0xB6);

	for(i = 0; i<30; i++) {
		delay_us(1000); // Wait  300 msfor wake up
	}

	while(isReadingCalibration())
		for(i = 0; i<10; i++) {
			delay_us(1000);
		}

	/* read calibration data */
	t1 = read16LE(DIG_T1);
	t2 = read16LE(DIG_T2);
	t3 = read16LE(DIG_T3);

	p1 = read16LE(DIG_P1);
	p2 = read16LE(DIG_P2);
	p3 = read16LE(DIG_P3);
	p4 = read16LE(DIG_P4);
	p5 = read16LE(DIG_P5);
	p6 = read16LE(DIG_P6);
	p7 = read16LE(DIG_P7);
	p8 = read16LE(DIG_P8);
	p9 = read16LE(DIG_P9);

	h1 = read8(DIG_H1);
	h2 = read16LE(DIG_H2);
	h3 = read8(DIG_H3);
	h4 = ((read8(DIG_H4) << 4 ) |  (read8(DIG_H4+1) & 0xF));
	h5 = ((read8(DIG_H5+1) << 4) | (read8(DIG_H5) >> 4));
	h6 = (int8_t) read8(DIG_H6);

	HumReg = read8(HUM_CONTROL);
	HumReg &= 0xF8;
	HumReg |= _huminidity_oversampling;
	write8(HUM_CONTROL, HumReg);
	HumReg = read8(HUM_CONTROL);
	write8(CONTROL, ((temperature_resolution<<5) | (pressure_oversampling<<2) | mode));

	if(mode == NORMALMODE)
	{
		setConfig(STANDBY_MS_0_5, FILTER_OFF);
	}
}

float Bme280::readTemperature(void)
{
  int32_t var1, var2;

  if(_mode == FORCEDMODE)
  {
	  uint8_t mode;
	  uint8_t ctrl = read8(CONTROL);
	  ctrl &= ~(0x03);
	  ctrl |= FORCEDMODE;
	  write8(CONTROL, ctrl);

	  mode = read8(CONTROL); 	// Read written mode
	  mode &= 0x03;							// Do not work without it...

	  if(mode == FORCEDMODE)
	  {
		  while(1) // Wait for end of conversion
		  {
			  mode = read8(CONTROL);
			  mode &= 0x03;
			  if(mode == SLEEPMODE)
				  break;
		  }
	  }
  }

  int32_t adc_T = read24(TEMPDATA);
  if (adc_T == 0x800000)
	  return -99.0;

  adc_T >>= 4;

  var1  = ((((adc_T>>3) - ((int32_t)t1 <<1))) *
		  ((int32_t)t2)) >> 11;

  var2  = (((((adc_T>>4) - ((int32_t)t1)) *
		  ((adc_T>>4) - ((int32_t)t1))) >> 12) *
		  ((int32_t)t3)) >> 14;

  t_fine = var1 + var2;

  float T  = (t_fine * 5 + 128) >> 8;
  return T/100.0;

}

int32_t Bme280::readPressure(void)
{
	int64_t var1, var2, p;

	// Must be done first to get the t_fine variable set up
	readTemperature();

	int32_t adc_P = read24(PRESSUREDATA);
	adc_P >>= 4;

	var1 = ((int64_t)t_fine) - 128000;
	var2 = var1 * var1 * (int64_t)p6;
	var2 = var2 + ((var1*(int64_t)p5)<<17);
	var2 = var2 + (((int64_t)p4)<<35);
	var1 = ((var1 * var1 * (int64_t)p3)>>8) +
			((var1 * (int64_t)p2)<<12);
	var1 = (((((int64_t)1)<<47)+var1))*((int64_t)p1)>>33;

	if (var1 == 0) {
		return 0;  // avoid exception caused by division by zero
	}
	p = 1048576 - adc_P;
	p = (((p<<31) - var2)*3125) / var1;
	var1 = (((int64_t)p9) * (p>>13) * (p>>13)) >> 25;
	var2 = (((int64_t)p8) * p) >> 19;

	p = ((p + var1 + var2) >> 8) + (((int64_t)p7)<<4);
	return (int32_t)p/256;
}

float Bme280::readHuminidity(void)
{
	// Must be done first to get the t_fine variable set up
	readTemperature();

	int32_t adc_H = read16(HUMIDDATA);
	if (adc_H == 0x8000) // value in case humidity measurement was disabled
	   return -99; //error
	int32_t v_x1_u32r;

	v_x1_u32r = (t_fine - ((int32_t)76800));

	  v_x1_u32r = (((((adc_H << 14) - (((int32_t)h4) << 20) -
					  (((int32_t)h5) * v_x1_u32r)) + ((int32_t)16384)) >> 15) *
				   (((((((v_x1_u32r * ((int32_t)h6)) >> 10) *
						(((v_x1_u32r * ((int32_t)h3)) >> 11) + ((int32_t)32768))) >> 10) +
					  ((int32_t)2097152)) * ((int32_t)h2) + 8192) >> 14));

	  v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) *
								 ((int32_t)h1)) >> 4));

	  v_x1_u32r = (v_x1_u32r < 0) ? 0 : v_x1_u32r;
	  v_x1_u32r = (v_x1_u32r > 419430400) ? 419430400 : v_x1_u32r;
	  float h = (v_x1_u32r>>12);
	  return  h / 1024.0;
}

int8_t Bme280::readTemperaturePressureHuminidity(float *temperature, int32_t *pressure, float *huminidity)
{
	int64_t var1, var2, p;

	// Must be done first to get the t_fine variable set up
	*temperature = readTemperature();

	if(*temperature == -99)
	  return -1;

	int32_t adc_P = read24(PRESSUREDATA);
	adc_P >>= 4;

	var1 = ((int64_t)t_fine) - 128000;
	var2 = var1 * var1 * (int64_t)p6;
	var2 = var2 + ((var1*(int64_t)p5)<<17);
	var2 = var2 + (((int64_t)p4)<<35);
	var1 = ((var1 * var1 * (int64_t)p3)>>8) +
			((var1 * (int64_t)p2)<<12);
	var1 = (((((int64_t)1)<<47)+var1))*((int64_t)p1)>>33;

	if (var1 == 0) {
		return 0;  // avoid exception caused by division by zero
	}
	p = 1048576 - adc_P;
	p = (((p<<31) - var2)*3125) / var1;
	var1 = (((int64_t)p9) * (p>>13) * (p>>13)) >> 25;
	var2 = (((int64_t)p8) * p) >> 19;

	p = ((p + var1 + var2) >> 8) + (((int64_t)p7)<<4);
	*pressure = (int32_t)p/256;

	// Pressure section
	int32_t adc_H = read16(HUMIDDATA);
	if (adc_H == 0x8000) // value in case humidity measurement was disabled
		return -1; //error

	int32_t v_x1_u32r;

	v_x1_u32r = (t_fine - ((int32_t)76800));

	v_x1_u32r = (((((adc_H << 14) - (((int32_t)h4) << 20) -
				  (((int32_t)h5) * v_x1_u32r)) + ((int32_t)16384)) >> 15) *
			   (((((((v_x1_u32r * ((int32_t)h6)) >> 10) *
					(((v_x1_u32r * ((int32_t)h3)) >> 11) + ((int32_t)32768))) >> 10) +
				  ((int32_t)2097152)) * ((int32_t)h2) + 8192) >> 14));

	v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) *
							 ((int32_t)h1)) >> 4));

	v_x1_u32r = (v_x1_u32r < 0) ? 0 : v_x1_u32r;
	v_x1_u32r = (v_x1_u32r > 419430400) ? 419430400 : v_x1_u32r;
	float h = (v_x1_u32r>>12);
	*huminidity = h / 1024.0;

	return 0;
}

float Bme280::readAltitude(float sea_level_pa) // standard: 1013.25 hPa;
{
	  float altitude;

	  float pressure = readPressure(); // in Si units for Pascal
	  pressure /= 100;

	  altitude = 44330 * (1.0 - pow(pressure / sea_level_pa, 0.1903));

	  return altitude;
}

} // namespace bme280

