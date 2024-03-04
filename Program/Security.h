int EncodeMode = 0;
int SecurityCheckInterval = 10000;
bool WaitingForSecurityReset = false; //After the timeout, the background thread will set bluetooth status to UNAUTHORIZED and set this bool to true. When the main thread sees this, it goes to unauthorized screen then sets this to false. Background thread will then see this and reset the bluetooth for a new connection.
String ConnectionPassword = "AF-ED-94-D1-2B-4F";
String GetBluetoothMac() 
{
  String output = "";
  uint8_t baseMac[6];
  esp_read_mac(baseMac, ESP_MAC_BT);
  for (int i = 0; i < 6; i++) {
    output += String(baseMac[i], HEX);
    if (i != 5) output += ":";
  }
  output.toUpperCase();
  return output;
}
String RandomHexCharacter(int value) {
  switch (value) {
    case 0: return "0";
    case 1: return "1";
    case 3: return "3";
    case 2: return "2";
    case 4: return "4";
    case 5: return "5";
    case 6: return "6";
    case 7: return "7";
    case 8: return "8";
    case 9: return "9";
    case 10: return "A";
    case 11: return "B";
    case 12: return "C";
    case 13: return "D";
    case 14: return "E";
    case 15: return "F";
    default: return "F";
  }
}
String RandomHexString(int value1, int value2) {
  return RandomHexCharacter(value1) + RandomHexCharacter(value2);
}
String EncodeBluetoothMac(String bluetoothMac) 
{
  randomSeed(analogRead(5));
  EncodeMode = random(6);
  //Shifting mac address
  int shiftIndex = EncodeMode * 3;
  String shiftedMacAddress = "";
  for (int i = 0; i < bluetoothMac.length(); i++) 
  {
    if (bluetoothMac[shiftIndex] == ':') 
    {
      shiftIndex++;
      continue;
    } 
    else if (shiftIndex < bluetoothMac.length()) 
    {
      shiftedMacAddress += bluetoothMac[shiftIndex];
      shiftedMacAddress += bluetoothMac[shiftIndex + 1];
      shiftedMacAddress += ':';
      shiftIndex += 2;
      i++;
    }
    if (shiftIndex >= bluetoothMac.length() - 1) shiftIndex = 0;
  }
  shiftedMacAddress = shiftedMacAddress.substring(0,shiftedMacAddress.length() -1);
  String encodedBluetoothMacAddress = RandomHexString(random(16), random(16)) + RandomHexString(0, EncodeMode);
  for (int i = 0; i < bluetoothMac.length(); i++) {
    if (shiftedMacAddress[i] == ':') encodedBluetoothMacAddress += RandomHexString(random(16), random(16));
    else encodedBluetoothMacAddress += shiftedMacAddress[i];
  }
  encodedBluetoothMacAddress += RandomHexString(random(16), random(16));
  return encodedBluetoothMacAddress;
}
String DecodeConnectionPassword(String password)
{
  String DecodedPassword = "";
  for(int i = 4,j = 0;i < password.length();i++)
  {
    if(j == 2) 
    {
      if(i >= password.length() - 2) break;
      DecodedPassword += "-";
      i ++;
      j = 0;
      continue;
    }
    else j++;
    DecodedPassword += password[i];
  }
  if(EncodeMode == 0) return DecodedPassword; //There is no shifting so you can return the decoded password directly
  String ShiftedDecodedPassword = "";
  int ShiftIndex = DecodedPassword.length() - (EncodeMode * 3) + 1;
  for(int i = 0;i < DecodedPassword.length();i++)
  {
    if(ShiftIndex == DecodedPassword.length()) 
    {
      ShiftIndex = 0;
      ShiftedDecodedPassword += '-';
      continue;
    }
    else ShiftedDecodedPassword += DecodedPassword[ShiftIndex];
    ShiftIndex ++;
  }
  return ShiftedDecodedPassword;
}
