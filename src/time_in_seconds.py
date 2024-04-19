import sys

def get_time_in_seconds(time_str):
  h, m, s = map(int, time_str.split(':'))
  return h * 3600 + m * 60 + s

if __name__ == "__main__":
  if len(sys.argv) != 2:
    print("Usage: python time_in_seconds.py <time>")
    sys.exit(1)

  time_str = sys.argv[1]
  time_in_seconds = get_time_in_seconds(time_str)
  print(f"Time in seconds: {time_in_seconds}")
  print(f"%70 = {time_in_seconds % 70}")