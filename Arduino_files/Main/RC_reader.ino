void ReadRCValues(){  // Reads RC channels
  
  int RC_count = 0;
  
  while (ppmReader.get(RC_count) != 0) {
    RC_channel_values[RC_count] = ppmReader.get(RC_count);
    RC_count++;
  }
}
