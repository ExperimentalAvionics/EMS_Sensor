
void configure_channels()
{
  uint8_t channel_number;
  uint32_t channel_assignment_data = 0;

  // ----- Channel 20: Assign Cold Junction Sensor (transistor used as a diode) -----
  channel_assignment_data =  channel_assignment_data =    SENSOR_TYPE__OFF_CHIP_DIODE |    DIODE_SINGLE_ENDED |    DIODE_NUM_READINGS__3 |    DIODE_AVERAGING_ON |  DIODE_CURRENT__80UA_320UA_640UA |    (uint32_t) 0x103200 << DIODE_IDEALITY_FACTOR_LSB;   // diode - ideality factor(eta): -- 0x103200
  assign_channel(TCS_CS_PIN, 20, channel_assignment_data);

  if (CYL == 4) { // 4 Cylinder engine board
    // 
    // ----------------------------------------------- EGT ---------------------------------------------------------
    channel_assignment_data = SENSOR_TYPE__TYPE_K_THERMOCOUPLE |    TC_COLD_JUNCTION_CH__20 |    TC_DIFFERENTIAL |    TC_OPEN_CKT_DETECT__YES |    TC_OPEN_CKT_DETECT_CURRENT__10UA;

    // ----- Channel 1: Assign  -----
    assign_channel(TCS_CS_PIN, 2, channel_assignment_data);
  
    // ----- Channel 2: Assign  -----
    assign_channel(TCS_CS_PIN, 4, channel_assignment_data);
  
    // ----- Channel 3: Assign  -----
    assign_channel(TCS_CS_PIN, 6, channel_assignment_data);
  
    // ----- Channel 4: Assign  -----
    assign_channel(TCS_CS_PIN, 8, channel_assignment_data);

/*
    // Use this section for J-type thermocouples as CHT sensors (usually aircooled engines)
    // ----------------------------------------------- CHT ---------------------------------------------------------
    channel_assignment_data = SENSOR_TYPE__TYPE_J_THERMOCOUPLE |    TC_COLD_JUNCTION_CH__20 |    TC_DIFFERENTIAL |    TC_OPEN_CKT_DETECT__YES |    TC_OPEN_CKT_DETECT_CURRENT__10UA;
  
    // ----- Channel 7: Assign  -----
    assign_channel(TCS_CS_PIN, 10, channel_assignment_data);
  
    // ----- Channel 8: Assign  -----
    assign_channel(TCS_CS_PIN, 12, channel_assignment_data);
  
    // ----- Channel 9: Assign  -----
    assign_channel(TCS_CS_PIN, 14, channel_assignment_data);
  
    // ----- Channel 10: Assign  -----
    assign_channel(TCS_CS_PIN, 16, channel_assignment_data);
*/

    // Use this section for Thermistors as CHT sensors (usually water-cooled engines like Rotax 912 etc)
    // This method is a bit of a hack
    // Normally thermistors require a calibrated "sense" resistor to get accurate reading.
    // In order to simplify the system, the thermistors will be treated like diode and its "temperature" output as a "resistance"
    // It will allow to use Steinhart-Hart formula to calculate actual tepmerature  (see the Get_CHT_Rotax() function)
    
    
    // ----------------------------------------------- CHT ---------------------------------------------------------
    channel_assignment_data = SENSOR_TYPE__OFF_CHIP_DIODE |    DIODE_SINGLE_ENDED |    DIODE_NUM_READINGS__2 |    DIODE_AVERAGING_OFF |    DIODE_CURRENT__20UA_80UA_160UA; 
  
    // ----- Channel 7: Assign  -----
    assign_channel(TCS_CS_PIN, 10, channel_assignment_data);
  
    // ----- Channel 8: Assign  -----
    assign_channel(TCS_CS_PIN, 12, channel_assignment_data);
  
    // ----- Channel 9: Assign  -----
    assign_channel(TCS_CS_PIN, 14, channel_assignment_data);
  
    // ----- Channel 10: Assign  -----
    assign_channel(TCS_CS_PIN, 16, channel_assignment_data);

  
  } else {   // 6 Cylinder engine board
    // ----------------------------------------------- EGT ---------------------------------------------------------
    channel_assignment_data = 0;
    channel_assignment_data = SENSOR_TYPE__TYPE_K_THERMOCOUPLE |    TC_COLD_JUNCTION_CH__20 |    TC_SINGLE_ENDED |    TC_OPEN_CKT_DETECT__YES |    TC_OPEN_CKT_DETECT_CURRENT__10UA;
    
    // ----- Channel 1: Assign  -----
    assign_channel(TCS_CS_PIN, 1, channel_assignment_data);
  
    // ----- Channel 2: Assign  -----
    assign_channel(TCS_CS_PIN, 2, channel_assignment_data);
  
    // ----- Channel 3: Assign  -----
    assign_channel(TCS_CS_PIN, 3, channel_assignment_data);
  
    // ----- Channel 4: Assign  -----
    assign_channel(TCS_CS_PIN, 4, channel_assignment_data);
  
    // ----- Channel 5: Assign  -----
    assign_channel(TCS_CS_PIN, 5, channel_assignment_data);
  
    // ----- Channel 6: Assign  -----
    assign_channel(TCS_CS_PIN, 6, channel_assignment_data);

    // For thermistor-based CHT see the code above (for the 4-cyl configuration
    // ----------------------------------------------- CHT ---------------------------------------------------------
    channel_assignment_data = SENSOR_TYPE__TYPE_J_THERMOCOUPLE |    TC_COLD_JUNCTION_CH__20 |    TC_SINGLE_ENDED |    TC_OPEN_CKT_DETECT__YES |    TC_OPEN_CKT_DETECT_CURRENT__10UA;
  
    // ----- Channel 7: Assign  -----
    assign_channel(TCS_CS_PIN, 7, channel_assignment_data);
  
    // ----- Channel 8: Assign  -----
    assign_channel(TCS_CS_PIN, 8, channel_assignment_data);
  
    // ----- Channel 9: Assign  -----
    assign_channel(TCS_CS_PIN, 9, channel_assignment_data);
  
    // ----- Channel 10: Assign  -----
    assign_channel(TCS_CS_PIN, 10, channel_assignment_data);
  
    // ----- Channel 11: Assign  -----
    assign_channel(TCS_CS_PIN, 11, channel_assignment_data);
  
    // ----- Channel 12: Assign  -----
    assign_channel(TCS_CS_PIN, 12, channel_assignment_data);
  }

}

void configure_global_parameters()
{
  // -- Set global parameters
  transfer_byte(TCS_CS_PIN, WRITE_TO_RAM, 0xF0, TEMP_UNIT__C |
                REJECTION__50_60_HZ);
  // -- Set any extra delay between conversions (in this case, 0*100us)
  transfer_byte(TCS_CS_PIN, WRITE_TO_RAM, 0xFF, 10);
}

//---------------------------------------   populate vars and arrays with  data ----------------------------------

void TCS_Read_All_Sensors() {
unsigned int Channel_Multiplier = 1;
unsigned int Channel_Max = 12;

if (CYL == 4) { // differential sensors for 4 cyl engine
  Channel_Multiplier = 2;
  Channel_Max = 16;
}
  
  if (channel_ready(TCS_CS_PIN)) {   // see if previously requested data is available to read
  
      get_channel_data(TCS_CS_PIN, TCS_Channel * Channel_Multiplier, TEMPERATURE);

//   Serial.print("Channel = ");
//   Serial.println(TCS_Channel * Channel_Multiplier);
//   Serial.print("Temp = ");
//   Serial.println(TCS_Result);
//   Serial.println("");


    if (CYL == 4) {  // 4 cyl engine board
      
      if (TCS_Channel == 1) {
           EGT[1] = TCS_Result;
           if (TCS_Error > 1) {
               EGT[1] = 0;
           }
      }
  
      if (TCS_Channel == 2) {
           EGT[2] = TCS_Result;
           if (TCS_Error > 1) {
               EGT[2] = 0;
           }
      }
  
      if (TCS_Channel == 3) {
           EGT[3] = TCS_Result;
           if (TCS_Error > 1) {
               EGT[3] = 0;
           }
      }
      if (TCS_Channel == 4) {
           EGT[4] = TCS_Result;
           if (TCS_Error > 1) {
               EGT[4] = 0;
           }
      }

      if (TCS_Channel == 5) {
//           CHT[1] = TCS_Result;
           CHT[1] = Get_CHT_Rotax(TCS_Result);
           if (TCS_Error > 1) {
               CHT[1] = 0;
           }
      }
      if (TCS_Channel == 6) {
//           CHT[2] = TCS_Result;
           CHT[2] = Get_CHT_Rotax(TCS_Result);
           if (TCS_Error > 1) {
               CHT[2] = 0;
           }
      }
      if (TCS_Channel == 7) {
//           CHT[3] = TCS_Result;
           CHT[3] = Get_CHT_Rotax(TCS_Result);
           if (TCS_Error > 1) {
               CHT[3] = 0;
           }
      }
      if (TCS_Channel == 8) {
//           CHT[4] = TCS_Result;
           CHT[4] = Get_CHT_Rotax(TCS_Result);
           if (TCS_Error > 1) {
               CHT[4] = 0;
           }
      }
      EGT[5] = 32767;
      EGT[6] = 32767;
      CHT[5] = 32767;
      CHT[6] = 32767;

    } else {  // 6 cyl engine board
      if (TCS_Channel == 1) {
           EGT[1] = TCS_Result;
           if (TCS_Error > 1) {
               EGT[1] = 0;
           }
      }
  
      if (TCS_Channel == 2) {
           EGT[2] = TCS_Result;
           if (TCS_Error > 1) {
               EGT[2] = 0;
           }
      }
  
      if (TCS_Channel == 3) {
           EGT[3] = TCS_Result;
           if (TCS_Error > 1) {
               EGT[3] = 0;
           }
      }
      if (TCS_Channel == 4) {
           EGT[4] = TCS_Result;
           if (TCS_Error > 1) {
               EGT[4] = 0;
           }
      }
      if (TCS_Channel == 5) {
           EGT[5] = TCS_Result;
           if (TCS_Error > 1) {
               EGT[5] = 0;
           }
      }
      if (TCS_Channel == 6) {
           EGT[6] = TCS_Result;
           if (TCS_Error > 1) {
               EGT[6] = 0;
           }
      }
      if (TCS_Channel == 7) {
           CHT[1] = TCS_Result;
           if (TCS_Error > 1) {
               CHT[1] = 0;
           }
      }
      if (TCS_Channel == 8) {
           CHT[2] = TCS_Result;
           if (TCS_Error > 1) {
               CHT[2] = 0;
           }
      }
      if (TCS_Channel == 9) {
           CHT[3] = TCS_Result;
           if (TCS_Error > 1) {
               CHT[3] = 0;
           }
      }
      if (TCS_Channel == 10) {
           CHT[4] = TCS_Result;
           if (TCS_Error > 1) {
               CHT[4] = 0;
           }
      }
      if (TCS_Channel == 11) {
           CHT[5] = TCS_Result;
           if (TCS_Error > 1) {
               CHT[5] = 0;
           }
      }
      if (TCS_Channel == 12) {
           CHT[6] = TCS_Result;
           if (TCS_Error > 1) {
               CHT[6] = 0;
           }
      }
    }
    
  TCS_Channel++;

 
  if (TCS_Channel * Channel_Multiplier > Channel_Max) {
    TCS_Channel = 1;
  }

  request_channel(TCS_CS_PIN, TCS_Channel * Channel_Multiplier);
}

}





void request_channel(uint8_t chip_select, uint8_t channel_number) 
{
  transfer_byte(chip_select, WRITE_TO_RAM, COMMAND_STATUS_REGISTER, CONVERSION_CONTROL_BYTE | channel_number);
}

void request_All_channels(uint8_t chip_select) 
{
  transfer_byte(chip_select, WRITE_TO_RAM, 0xF7, 0xFF);
  transfer_byte(chip_select, WRITE_TO_RAM, COMMAND_STATUS_REGISTER, CONVERSION_CONTROL_BYTE);
}


bool channel_ready(uint8_t chip_select)
{
  uint8_t process_finished = 0;
  uint8_t data;
    data = transfer_byte(chip_select, READ_FROM_RAM, COMMAND_STATUS_REGISTER, 0);
    process_finished  = data & 0x40;
    if (process_finished == 0) {
      return false;
    } else {
      return true;
    }
}

void get_channel_data(uint8_t chip_select, uint8_t channel_number, uint8_t channel_output)
{
  uint32_t raw_data;
  uint8_t fault_data;
  uint16_t start_address = get_start_address(CONVERSION_RESULT_MEMORY_BASE, channel_number);
  uint32_t raw_conversion_result;

  int32_t signed_data;
  float scaled_result;

  raw_data = transfer_four_bytes(chip_select, READ_FROM_RAM, start_address, 0);



  // 24 LSB's are conversion result
  raw_conversion_result = raw_data & 0xFFFFFF;
  

  // 8 MSB's show the fault data
  fault_data = raw_data >> 24;
  fault_data = fault_data & ~SENSOR_ABOVE; // exclude sensor above from the error to accomodate Rotax CHT hack
  TCS_Error = fault_data;

  
  
 // print_fault_data(fault_data);

// Error debuging code:
/*
  if (TCS_Error>1) {

  Serial.print(F("\nChannel "));
  Serial.println(channel_number);
  Serial.print(F("  FAULT DATA = "));
  Serial.println(fault_data, BIN);

 
 if (fault_data & SENSOR_HARD_FAILURE) {
    Serial.println(F("  - SENSOR HARD FALURE"));
 }
  if (fault_data & ADC_HARD_FAILURE)
    Serial.println(F("  - ADC_HARD_FAILURE"));
  if (fault_data & CJ_HARD_FAILURE)
    Serial.println(F("  - CJ_HARD_FAILURE"));
  if (fault_data & CJ_SOFT_FAILURE)
    Serial.println(F("  - CJ_SOFT_FAILURE"));
  if (fault_data & SENSOR_ABOVE)
    Serial.println(F("  - SENSOR_ABOVE"));
  if (fault_data & SENSOR_BELOW)
    Serial.println(F("  - SENSOR_BELOW"));
  if (fault_data & ADC_RANGE_ERROR)
    Serial.println(F("  - ADC_RANGE_ERROR"));
  if (!(fault_data & VALID))
    Serial.println(F("INVALID READING !!!!!!"));
  if (fault_data == 0b11111111)
    Serial.println(F("CONFIGURATION ERROR !!!!!!"));
  }
*/


  signed_data = raw_conversion_result;
  

  // Convert the 24 LSB's into a signed 32-bit integer
  if (signed_data & 0x800000)
    signed_data = signed_data | 0xFF000000;

  // Translate and print result
  if (channel_output == TEMPERATURE)
  {
    scaled_result = float(signed_data) / 1024;
    //Serial.print(F("  Temperature = "));
    //Serial.println(scaled_result);
  }
  else if (channel_output == VOLTAGE)
  {
    scaled_result = float(signed_data) / 2097152;
    //Serial.print(F("  Direct ADC reading in V = "));
    //Serial.println(scaled_result);
  }


  //Serial.print(F("  Result = "));
  //Serial.println(scaled_result);
  
  TCS_Result = scaled_result;
}
