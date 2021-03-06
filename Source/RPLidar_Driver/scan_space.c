#include "scan_space.h"


Field_TypeDef Danger;
Field_TypeDef Warring;

uint8_t flag_Danger=0;
uint8_t flag_Warring=0;

uint8_t step_scan=0;
uint8_t wait_scan=1;



void led_alert_init(void)
{
	GPIO_InitTypeDef  Led_InitStructure; 
	RCC_APB2PeriphClockCmd(LED_CLK, ENABLE);
	
	Led_InitStructure.GPIO_Pin = LED_PIN;
	Led_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	Led_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(LED_PORT, &Led_InitStructure);
}

/*
 * @brief: get barrier position
 * @param: pointer to barrier's coordinate
 * @param: radius to scan
 * @param: angle to scan
 * @retval: none
 */
void get_pos(CoBarrier_TypeDef* brr, float _r, float ang)
{
	brr->x_pos = _r*cos(ang);
  	brr->y_pos = _r*sin(ang);
	brr->_r	   = _r;
}


/*
 * @brief: check barrier in field
 * @param: pointer to field
 * @param: pointer to barrier's coordinate
 * @retval: +True: barrier in field
 * 			+False: barrier not in field
 */
uint8_t barrier_in_field(Field_TypeDef* field, CoBarrier_TypeDef* brr)
{
	uint8_t status_scan = 0;
	
	if(fabs(brr->x_pos) <= (field->border_width/2)
		&& fabs(brr->y_pos) <= field->border_height)
	{
		/* barrier in field */
		status_scan=1;
	}
	
	return status_scan;
}


/*
 * @brief: check field's angle  
 * @param: pointer to field
 * @param: value angle
 * @retval: +True: angle in field
 * 			+False: angle not in field
 */
uint8_t is_angle_in_field(Field_TypeDef* field, float angle)
{
	uint8_t result=0;
	float angle1;
	float angle2;
	
	//we will split angle scan field into two to process easier
	angle2 = field->angle_range; // angle left
	angle1 = 360-field->angle_range; //angle right
	
	if(angle <= angle2 || angle >= angle1)
	{
		//angle in field
		result =1;
	}

	return result;
}


/*
 * @brief: scan danger field
 * @param: barrier's distance 
 * @param: barrier's angle
 * @param: quality of the current measurement
 * @retval: none
 */
void scan_danger_space(float distance, float angle, uint8_t quality)
{
	CoBarrier_TypeDef barrier;
	
//	if(quality > 0)
//	{
		if(is_angle_in_field(&Danger,angle))
		{
			get_pos(&barrier,distance,angle); // get position barrier

			if(barrier_in_field(&Danger,&barrier))//barrier in field danger?
			{
				Danger.rate_field++; // rate field danger
			}
		}
//	}
}


/*
 * @brief: scan warring field
 * @param: barrier's distance 
 * @param: barrier's angle
 * @param: quality of the current measurement
 * @retval: none
 */
void scan_warring_space(float distance, float angle, uint8_t quality)
{
	CoBarrier_TypeDef barrier;
	
//	if(quality > 0)
//	{
		if(is_angle_in_field(&Warring,angle))
		{
			get_pos(&barrier,distance,angle);// get position barrier

			if(barrier_in_field(&Warring,&barrier)) //barrier in field warring?
			{
				Warring.rate_field++; // rate field warring
			}
		}
//	}
}


/*
 * @brief: parse data field scanned
 * @param: none
 * @retval: none
 */
void parseData(void)
{
	if(Danger.rate_field > RATE_OF_DANGER)
	{
		flag_Danger =1;
	}
	else
	{
		flag_Danger =0;
	}
	
	if(Warring.rate_field > RATE_OF_WARNING)
	{
		flag_Warring =1;
	}
	else
	{
		flag_Warring =0;
	}
	
	Danger.rate_field=0;
	Warring.rate_field=0;
}

/*
 * @brief: process out data bits. This function help delay change status output data bits
 * @param: poiter to output data bits
 * @param: flag compare stattus data output bit
 * @param: time delay compare
 * @retval: status output
 */
uint8_t out_data(outdata_t* output, uint8_t flag, uint16_t dlyTicks)
{
	
	if(output->sst != flag)
	{
		if(!output->curtTicks)
			output->curtTicks = millis();
		
		if((unsigned long)(millis() - output->curtTicks) > dlyTicks)
		{
			output->sst = flag;
		}
	}
	else
	{
		output->curtTicks =0;
	}
	
	return output->sst;
}


/*
 * @brief: process scan space
 * @param: none
 * @retval: none
 */
void scan_space(void)
{
	float distance; //distance value in mm unit
	float angle; //anglue value in degree
	bool startBit; //whether this point is belong to a new scan
	uint8_t quality; //quality of the current measurement
	
	distance = rplidar_get_distance(); 	//distance value in mm unit
	angle    = rplidar_get_angle(); 		//anglue value in degree
	startBit = rplidar_get_startBit(); 	//whether this point is belong to a new scan
	quality  = rplidar_get_quality(); 	//quality of the current measurement
	
	//perform data processing here... 
	switch (step_scan)
	{
		case 0:
			if(startBit)
			{
				scan_danger_space(distance, angle, quality);
				scan_warring_space(distance, angle, quality);
				step_scan++;
			}
			break;
		case 1:
			if(!startBit)
			{
				scan_danger_space(distance, angle, quality);
				scan_warring_space(distance, angle, quality);
			}
			else step_scan++;
			break;
		case 2:
			parseData();
			step_scan = 0;
			break;
		default:
			break;
	}
}


/*
 * @brief: initialize rplidar
 * @param: none
 * @retval: none
 */
void lidar_init(void)
{	
	Warring.angle_range=10;
	Warring.border_height = 10000;
	Warring.border_width = 1000;
	Warring.rate_field =15;
	
	Danger.angle_range=30;
	Danger.border_height = 500;
	Danger.border_width = 250;
	Danger.rate_field =10;
	
	rplidar_begin();
}


/*
 * @brief: process rplidar scanning
 * @param: none
 * @retval: none
 */
void lidar_scanning(void)
{
	static outdata_t outDanger;
	static outdata_t outWarring;
	static uint8_t event_scan=0;
	static uint32_t last_timeEV=0;
	u_result status_scan;
	unsigned long danger_dlyTicks=0;
	unsigned long warring_dlyTicks=0;
	
	
	switch(event_scan)
	{
		case 0: // scanning
		{
			MOTOR_CTRL =1;
			status_scan =	rplidar_waitPoint(RPLIDAR_DEFAULT_TIMEOUT); // wait for sample point to arrive
			
			if (status_scan == RESULT_OK) 
			{
				scan_space(); // start scan
			}
			else if(status_scan == RESULT_OPERATION_TIMEOUT) //rplidar error
			{
				event_scan++;
			}
			break;
		}
		case 1:// try to detect rplidar
		{
//			MOTOR_CTRL = 0;
			if(IS_OK(detect_lidar())) 
			{
//				MOTOR_CTRL =1;
				event_scan++;
				last_timeEV = millis();
			}
			break;
		}
		case 2:
		{
			// delay for rplidar restart
			if((uint32_t)(millis() - last_timeEV) > 1000)
			{
				event_scan=0;;
			}
			break;
		}
		default:
			event_scan=0;
			break;
	}
	

	// set time delay compare status danger field
	if(flag_Danger)
	{
		danger_dlyTicks = 1000;
	}
	else
	{
		danger_dlyTicks = 10;
	}
	
	// set time delay compare status warring field
	if(flag_Warring)
	{
		warring_dlyTicks = 1000;
	}
	else
	{
		warring_dlyTicks = 10;
	}
	
	// control out data
	if(out_data(&outDanger,flag_Danger,danger_dlyTicks))
	{
		//Danger
		LED_DANGER=1;
		LED_WARRING = 0;
		LED_SAFETY = 0;
	}
	else if (out_data(&outWarring,flag_Warring,warring_dlyTicks))
	{
		//Warring
		LED_DANGER=0;
		LED_WARRING = 1;
		LED_SAFETY = 0;
	}
	else
	{
		//SAFE
		LED_DANGER =0;
		LED_WARRING = 0;
		LED_SAFETY =1;
	}
}

