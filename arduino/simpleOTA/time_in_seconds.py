# To run: python time_in_seconds.py hours:minutes:seconds

import sys

def get_time_in_seconds(time_str):
  try:
    h, m, s = map(int, time_str.split(':'))
    return h * 3600 + m * 60 + s
  except ValueError:
    print("Invalid time format. Please use the format hours:minutes:seconds.")
    sys.exit(1)

weekday_offsets = {
    'Monday': 60,
    'Tuesday': 40,
    'Wednesday': 20,
    'Thursday': 0,
    'Friday': 50,
    'Saturday': 30,
    'Sunday': 10
}

if __name__ == "__main__":
  if len(sys.argv) != 2:
    print("Usage: python time_in_seconds.py hours:minutes:seconds")
    sys.exit(1)

  time_str = sys.argv[1]
  time_in_seconds = get_time_in_seconds(time_str)
  remainder = time_in_seconds % 70
  print(f"Time in seconds: {time_in_seconds}")
  print(f"%70 = {remainder}")

  for day, offset in weekday_offsets.items():
    if offset == remainder:
      print(f"The expected day: {day}")