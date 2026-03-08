import time
import threading

class Heartbeat:
    def __init__(self, interval_seconds=5, callback=None):
        self.interval = interval_seconds
        self.callback = callback
        self._running = False
        self._thread = None

    def _heartbeat_loop(self):
        while self._running:
            if self.callback:
                self.callback()
            time.sleep(self.interval)

    def start(self):
        if not self._running:
            self._running = True
            self._thread = threading.Thread(target=self._heartbeat_loop)
            self._thread.daemon = True # Allow program to exit even if thread is running
            self._thread.start()
            print(f"Heartbeat started with interval of {self.interval} seconds.")

    def stop(self):
        if self._running:
            self._running = False
            if self._thread:
                self._thread.join() # Wait for the thread to finish
            print("Heartbeat stopped.")

# Example usage:
def my_heartbeat_function():
    print(f"Heartbeat tick! Current time: {time.ctime()}")

if __name__ == "__main__":
    heartbeat_monitor = Heartbeat(interval_seconds=0.01, callback=my_heartbeat_function)
    heartbeat_monitor.start()

    # Keep the main thread alive for a while to observe the heartbeat
    try:
        time.sleep(10)
    except KeyboardInterrupt:
        pass
    finally:
        heartbeat_monitor.stop()

