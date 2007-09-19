def run():
  import Online.StreamingComponent as Streaming
  Streaming.execute('-type Storage -wait'.split(' '))
if __name__ == "__main__":
  run()
