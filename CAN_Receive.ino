void CAN_Receive() {
  unsigned char buf[8];
  unsigned char len = 0;
   unsigned char ext = 0;

  if(CAN_MSGAVAIL == CAN.checkReceive())            // check if data coming
    {
  //      CAN.readMsgBuf(&len, buf);    // read data,  len: data length, buf: data buf

        // unsigned char canId = CAN.getCanId();
        
        CAN.readMsgBuf(&canId, &ext, &len, buf);

        Serial.println("-----------------------------");
        Serial.println("get data from ID: ");
        Serial.println(canId);

        for(int i = 0; i<len; i++)    // print the data
        {
            Serial.print(buf[i]);
            Serial.print("\t");
         }
        Serial.println();
    }

}

