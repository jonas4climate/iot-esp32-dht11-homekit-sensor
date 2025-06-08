# Blynk IoT Setup Guide

This guide will help you set up Blynk IoT platform integration for your ESP32 Climate Sensor.

## What is Blynk?

Blynk is an IoT platform that allows you to monitor and control your devices remotely through mobile apps and web dashboards. With this integration, you can:

- Monitor temperature, humidity, and heat index in real-time
- Receive alerts when values exceed thresholds
- View historical data and trends
- Access your sensor data from anywhere in the world

## Step 1: Create Blynk Account

1. Go to [blynk.cloud](https://blynk.cloud)
2. Sign up for a free account
3. Verify your email address

## Step 2: Create Template

1. Log into Blynk Console
2. Go to **Developer Zone** → **My Templates**
3. Click **"+ New Template"**
4. Fill in the details:
   - **Name**: `ESP32 Climate Monitor`
   - **Hardware**: `ESP32`
   - **Connection Type**: `WiFi`
   - **Template Description**: `Temperature and humidity monitoring with heat index calculation`
5. Click **Done**

## Step 3: Configure Datastreams

In your template, go to **Datastreams** tab and create the following:

### Virtual Pin V1 - Temperature
- **Name**: `Temperature`
- **Pin**: `V1`
- **Data Type**: `Double`
- **Units**: `°C`
- **Min**: `-40`
- **Max**: `85`
- **Default Value**: `20`

### Virtual Pin V2 - Humidity  
- **Name**: `Humidity`
- **Pin**: `V2`
- **Data Type**: `Double`
- **Units**: `%`
- **Min**: `0`
- **Max**: `100`
- **Default Value**: `50`

### Virtual Pin V3 - Heat Index
- **Name**: `Heat Index`
- **Pin**: `V3`
- **Data Type**: `Double`
- **Units**: `°C`
- **Min**: `-40`
- **Max**: `85`
- **Default Value**: `20`

### Virtual Pin V4 - Status
- **Name**: `Sensor Status`
- **Pin**: `V4`
- **Data Type**: `String`
- **Default Value**: `"Offline"`

## Step 4: Get Your Credentials

1. **Template ID**: Copy from the template info section (starts with "TMPL")
2. **Create Device**:
   - Go to **Search** → **My Devices**
   - Click **"+ New Device"**
   - Choose **"From Template"**
   - Select your ESP32 Climate Monitor template
   - Give it a name (e.g., "Living Room Sensor")
   - Click **Create**
3. **Auth Token**: Copy the device auth token from device info

## Step 5: Update Configuration

Edit `include/config.h` in your project:

```cpp
// Blynk Configuration
#define BLYNK_ENABLED true
#define BLYNK_TEMPLATE_ID "TMPL4xxxx"  // Your Template ID
#define BLYNK_TEMPLATE_NAME "ESP32 Climate Monitor"
#define BLYNK_AUTH_TOKEN "your_auth_token_here"  // Your Device Auth Token
```

## Step 6: Create Mobile App Dashboard

1. Download Blynk IoT app from App Store/Google Play
2. Log in with your account
3. Open your device
4. Click **"Edit Dashboard"** (wrench icon)
5. Add widgets:

### Temperature Gauge
- Widget: **Gauge**
- Datastream: **Temperature (V1)**
- Style: Customize colors and ranges

### Humidity Gauge  
- Widget: **Gauge**
- Datastream: **Humidity (V2)**
- Style: Customize colors and ranges

### Heat Index Display
- Widget: **Value Display** or **Gauge**
- Datastream: **Heat Index (V3)**
- Style: Customize appearance

### Status Label
- Widget: **Label**
- Datastream: **Sensor Status (V4)**
- Shows sensor online/offline status

### Optional: Charts
- Widget: **SuperChart**
- Add multiple datastreams for historical data
- Configure time ranges (1 hour, 1 day, 1 week)

## Step 7: Set Up Notifications (Optional)

1. In Blynk Console, go to your device
2. Go to **Automations** tab
3. Create automations for:
   - High temperature alerts
   - Low/high humidity warnings
   - Sensor offline notifications

Example automation:
- **Condition**: Temperature > 30°C
- **Action**: Send notification "High temperature detected!"

## Step 8: Test Your Setup

1. Upload the code to your ESP32
2. Open Serial Monitor to verify Blynk connection
3. Check your mobile app dashboard
4. Verify data is updating every 60 seconds

## Troubleshooting

### Connection Issues
- Verify WiFi credentials are correct
- Check that Template ID and Auth Token match exactly
- Ensure stable internet connection

### Data Not Updating
- Check virtual pin assignments match configuration
- Verify sensor is working (check serial output)
- Confirm Blynk.run() is called regularly in loop()

### App Issues
- Force close and reopen the app
- Check internet connection on mobile device
- Verify you're logged into the correct account

## Features

### Real-time Monitoring
- Data updates every 60 seconds
- Instant notifications for alerts
- Live status indicators

### Historical Data
- Automatic data logging
- Export capabilities
- Trend analysis

### Remote Access
- Monitor from anywhere with internet
- Multiple device support
- Family sharing options

## Cost

- **Free Tier**: Up to 5 devices, basic features
- **Paid Plans**: More devices, advanced features, longer data retention

For most home monitoring setups, the free tier is sufficient.
