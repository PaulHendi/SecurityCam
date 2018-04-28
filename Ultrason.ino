// Flag to help debug; checking if the filtered distance is Okay
// Need to keep it to 0 when sending info via bluetooth
int DEBUG = 0;

// Ultrasound sensor's trigger and echo are connected to pin 2 and 3
const int Trigger_pin = 2;
const int Echo_pin = 3;

// An array to keep last 11 distances => This will be useful to filter the distance
const int array_size = 11;
int DistanceArray[array_size] =  {0,0,0,0,0,0,0,0,0,0,0};


// Setup function : 
//   _ Seting the baud rate to 115200 (note that RX and TX are used for the bluetooth)
//   _ Setting the pin mode of both pin used with the ultrasound sensor
void setup() {
  Serial.begin(115200);
  pinMode(Trigger_pin,OUTPUT);
  pinMode(Echo_pin, INPUT);
}


// In order to use a median filter, we need to sort the DistanceArray.
// For this matter we use the insertion sort algorithm
void sort() 
{
  int i = 1;
  int j = i;
  
  while (i < array_size){
  
    j = i;
    
    while (j>0 and DistanceArray[j-1]>DistanceArray[j]){
    
       // swap DistanceArray[j] and DistanceArray[j-1]
       int tmp = DistanceArray[j];
       DistanceArray[j] = DistanceArray[j-1];
       DistanceArray[j-1] = tmp;  
       
       j = j-1;
    }
    i = i + 1;
  }
}


// The median filter's purpose is to not take into account extreme measures like an average filter would.
// And the problem of having extrem is quite common with the ultrasound sensor. The goal is to take the 5th 
// mesures in the DistanceArray
int MedianFilter(int Distance)
{
  static int count = 0;  // static so that at every call it keeps incrementing
  
  // Here we insert new distance at the top of the array and shift elements by one index. 
  DistanceArray[0] = Distance; 
  for (int i = 10;i>0;i--)
  {
    DistanceArray[i] = DistanceArray[i-1]; 
  }
  
  // If we have 11 measures, which is equivalent to saying if we called this function 11 times
  if (count>=11)
  {
    // We can sort the array and return the median 
    sort();
    return DistanceArray[5];  
  }
  
  // if not wee keep incrementing the counter and return current distance
  count++;
  return Distance;
  
}



// Main loop
void loop() {

  // Those lines below correspond to the process of getting the measure from the sensor (see datasheet for more details)
  digitalWrite(Trigger_pin,HIGH);            // send a "1" on the trigger pin
  delayMicroseconds(10);                     // wait 10 microseconds 
  digitalWrite(Trigger_pin,LOW);             // send a "0" on the trigger pin
  int delayEcho = pulseIn(Echo_pin,HIGH);    // Wait for the answer from the sensor and keep the time it took
  int Distance =  delayEcho * 0.033/ 2;      // Basically : distance = time*speed/2 = time * 0.033 / 2 cm
  
  
  // Now we use our filter defined above
  int DistanceFiltered = MedianFilter(Distance);
  
  
  // If the distance is higher than 1m50 than we send a 1 (this will be understood like "There is someone here !")
  if (DistanceFiltered < 150 & DEBUG == 0) 
  {
    Serial.print(1);
  }
  else if (DistanceFiltered >= 150 & DEBUG == 0)   
  {
    Serial.print(0);
  }
  
  
  // In case we want to debug using arduino's software only.
  // This should not be used with the communication by bluetooth, it wouldn't be understood
  if (DEBUG) 
  { 
    Serial.print(Distance); Serial.print("\t");
    Serial.println(DistanceFiltered);
  }
  
  // We need to wait some time before asking for a new measurement again
  delayMicroseconds(100-delayEcho-10); 
}
