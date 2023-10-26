# External Libraries
import tkinter as tk
import cv2
from PIL import Image, ImageTk
import threading

# Local Modules
from lsl import get_speed_from_stream


class VideoPlaybackWindow(tk.Toplevel):
    def __init__(self, video_path, stream, parent):
        super().__init__()

        # Basic parameters
        self.parent = parent
        self.title("BrainStart")
        
        self.play_forward = True

        # Initialize video canvas
        self.video_canvas = tk.Canvas(self, bg="black")
        self.video_canvas.pack(fill="both", expand=True)

        # Close window event
        self.protocol("WM_DELETE_WINDOW", self.on_close)

        # Playback speed initialization
        self.playback_speed = 1.0

        # Create and place speed rectangles
        self._create_speed_rectangles()

        # Store widget reference
        self.parent.video_canvas = self.video_canvas

        # Parameters for speed update thread
        self.is_running = True
        self.speed_thread = threading.Thread(target=self.update_speed, args=(stream,))
        self.speed_thread.daemon = True
        self.speed_thread.start()

        self.frame_delay = 5  # Delay between frames

        # Start video playback process
        self.play_video(video_path, stream)

    def _create_speed_rectangles(self):
        self.rectangle_width = 30
        self.rectangle_height = 200
        self.fill_percentage = 0.5

        self.outer_rectangle = self.video_canvas.create_rectangle(
            10, 10,
            10 + self.rectangle_width, 10 + self.rectangle_height,
            outline="white", width=2
        )
        self.inner_rectangle = self.video_canvas.create_rectangle(
            10, 10 + self.rectangle_height * self.fill_percentage,
            10 + self.rectangle_width, 10 + self.rectangle_height,
            fill="green"
        )

    def update_speed(self, stream):
        while self.is_running:
            new_speed = get_speed_from_stream(stream)
            if new_speed is not None:
                max_delay = 1000
                self.playback_speed = new_speed
                
                
                if new_speed < 20:
                    self.play_forward = False
                    self.frame_delay = 1#max(1, int(max_delay / self.playback_speed))
                    
                else:
                    self.play_forward = True
                    self.frame_delay = 1#max(1, int(max_delay / self.playback_speed))
                    
                self.fill_percentage = self.playback_speed / max_delay

                
                
                print('Frame Delay: ', self.frame_delay, 'ms.')

    def play_video(self, video_path, stream):
        self.cap = cv2.VideoCapture(video_path)
        self.total_frames = self.cap.get(cv2.CAP_PROP_FRAME_COUNT)

        ret, frame = self.cap.read()
        frame_rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
        im = Image.fromarray(frame_rgb)
        self.imgtk = ImageTk.PhotoImage(image=im)
        self.canvas_img = self.video_canvas.create_image(0, 0, anchor="nw", image=self.imgtk)

        self.percentage_text = self.video_canvas.create_text(
            self.video_canvas.winfo_width() - 15,
            self.video_canvas.winfo_height() - 15,
            fill="white",
            anchor="se"
        )

        #self.play_forward = forward
        self.update_video_frame()

    def update_video_frame(self):
        if not self.video_canvas.winfo_exists():
            self.cap.release()
            return

        ret, frame = self.cap.read()
        if not ret:
            self.cap.release()
            return

        frame_rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)

        canvas_width = self.video_canvas.winfo_width()
        canvas_height = self.video_canvas.winfo_height()
        im = Image.fromarray(frame_rgb).resize((canvas_width, canvas_height))
        imgtk = ImageTk.PhotoImage(image=im)

        self.video_canvas.itemconfig(self.canvas_img, image=imgtk)
        self.video_canvas.imgtk = imgtk

        current_frame = self.cap.get(cv2.CAP_PROP_POS_FRAMES)
        percentage = (current_frame / self.total_frames) * 100
        self.video_canvas.itemconfig(self.percentage_text, text=f"{percentage:.2f}%")

        self.video_canvas.coords(self.percentage_text,
                                 self.video_canvas.winfo_width() - 15,
                                 self.video_canvas.winfo_height() - 15)

        # Determine frame direction (forward or backward)
        if self.play_forward:
            current_frame += 1
        else:
            current_frame -= 2

        # Check if we reached the end of the video
        if self.play_forward and current_frame >= self.total_frames:
            current_frame = 0
        elif not self.play_forward and current_frame < 0:
            current_frame = self.total_frames - 1
        if self.play_back and current_frame <= 0:
            current_frame = 0

        # Set the new frame position
        self.cap.set(cv2.CAP_PROP_POS_FRAMES, current_frame)

        # new_speed = 1#get_speed_from_stream(stream)
        #if new_speed is not None:
        #max_delay = 1000
        #self.playback_speed = new_speed
        self.fill_percentage = self.playback_speed / self.frame_delay

        #self.frame_delay = max(1, int(max_delay / self.playback_speed))
        

        self.video_canvas.after(self.frame_delay, self.update_video_frame)

    def on_close(self):
        self.parent.video_canvas = None
        self.is_running = False
        self.destroy()
