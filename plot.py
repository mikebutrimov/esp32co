import requests
import matplotlib.pyplot as plt
from datetime import datetime, timedelta
import pandas as pd

interval = 60

# API endpoint
url = "http:/YOUR_ESP32_IP/log"


def get_sensor_data():
    try:
        response = requests.get(url)
        if response.status_code == 200:
            return response.text
        else:
            print(f"Failed to get sensor data. Status code: {response.status_code}")
            return None
    except requests.RequestException as e:
        print(f"Request error: {e}")
        return None

def parse_sensor_data(sensor_data):
    lines = sensor_data.strip().split('\n')
    data = [line.split(';') for line in lines]
    data.reverse()
    df = pd.DataFrame(data, columns=['CO2', 'Humidity', 'Temperature'], dtype=float)
    return df

def plot_sensor_data(time_deltas, values, ylabel, title):
    plt.plot(time_deltas, values, marker='o', label=ylabel)

def main():
    # Get sensor data
    sensor_data = get_sensor_data()

    if sensor_data is not None:
        # Parse sensor data
        df = parse_sensor_data(sensor_data)

        # Get current timestamp
        current_time = datetime.now()

        # Create time deltas for each data point based on the interval
        time_deltas = [int((current_time - timedelta(seconds=i * interval)).timestamp()) for i in range(len(df))]

        # Convert time deltas to hours and minutes
        time_deltas = [(current_time - timedelta(seconds=i * interval)).strftime('%H:%M:%S') for i in range(len(df)-1, -1, -1)]

        # Calculate time difference between the most recent and oldest data point
        time_difference = (datetime.strptime(time_deltas[-1], '%H:%M:%S') - datetime.strptime(time_deltas[0], '%H:%M:%S')).total_seconds()

        # Calculate an appropriate tick frequency for the X-axis
        tick_frequency = max(int(abs(time_difference)/(interval*10)), 1)

        # Plot each graph in a separate non-blocking window
        plt.figure(figsize=(10, 6))

        # Plot CO2
        plot_sensor_data(time_deltas, df['CO2'][::-1], 'CO2 (ppm)', 'CO2 Levels Over Time')
        plt.title('CO2 Levels Over Time')
        plt.xlabel('Time (HH:MM)')
        plt.ylabel('CO2 (ppm)')
        plt.xticks(rotation=45)
        plt.legend()
        plt.tight_layout()
        plt.gca().xaxis.set_major_locator(plt.MaxNLocator(integer=True))  # Force integer ticks on X-axis
        plt.gca().xaxis.set_major_locator(plt.MultipleLocator(tick_frequency))  # Adjust tick frequency
        plt.show(block=False)

        # Plot Humidity
        plt.figure(figsize=(10, 6))
        plot_sensor_data(time_deltas, df['Humidity'][::-1], 'Humidity (%)', 'Humidity Levels Over Time')
        plt.title('Humidity Levels Over Time')
        plt.xlabel('Time (HH:MM)')
        plt.ylabel('Humidity (%)')
        plt.xticks(rotation=45)
        plt.legend()
        plt.tight_layout()
        plt.gca().xaxis.set_major_locator(plt.MaxNLocator(integer=True))  # Force integer ticks on X-axis
        plt.gca().xaxis.set_major_locator(plt.MultipleLocator(tick_frequency))  # Adjust tick frequency
        plt.show(block=False)

        # Plot Temperature
        plt.figure(figsize=(10, 6))
        plot_sensor_data(time_deltas, df['Temperature'][::-1], 'Temperature (°C)', 'Temperature Over Time')
        plt.title('Temperature Over Time')
        plt.xlabel('Time (HH:MM)')
        plt.ylabel('Temperature (°C)')
        plt.xticks(rotation=45)
        plt.legend()
        plt.tight_layout()
        plt.gca().xaxis.set_major_locator(plt.MaxNLocator(integer=True))  # Force integer ticks on X-axis
        plt.gca().xaxis.set_major_locator(plt.MultipleLocator(tick_frequency))  # Adjust tick frequency
        plt.show()

if __name__ == "__main__":
    main()

