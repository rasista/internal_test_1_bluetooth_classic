class sbc_frame:
    def __init__(self):
        self.sampling_freq = 44100
        self.channel_mode = "joint_stereo"
        self.block_size = 16
        self.sub_bands = 8
        self.bitpool = 35
        self.frame_length = self.get_frame_length()
        self.audio_duration = self.get_audio_duration()

    def get_frame_length(self):
        if(self.channel_mode == "mono"):
            nrof_channels = 1
        else:
            nrof_channels = 2
        if(self.channel_mode == "mono" or self.channel_mode == "dual"):
            frame_length = 4 + (4 * self.sub_bands * nrof_channels) // \
                8 + (self.block_size * nrof_channels * self.bitpool // 8)
        else:
            frame_length = 4 + (4 * self.sub_bands * nrof_channels) // 8
            if(self.channel_mode == "joint_stereo"):
                frame_length += ((self.sub_bands +
                                       self.block_size * self.bitpool) // 8)
            else:
                frame_length += ((self.block_size * self.bitpool) // 8)
        return frame_length

    def get_audio_duration(self):
        return (self.block_size * self.sub_bands / self.sampling_freq * 1000)

if __name__ == "__main__":
    import argparse

    parser = argparse.ArgumentParser(description = "Calculates SBC frame size")
    parser.add_argument('-f', type = int, help='Sampling frequency')
    parser.add_argument('-m', type = str, help='Channel Mode, mono/dual/stereo/joint_stereo')
    parser.add_argument('-s', type = int, help='Number of Sub Bands')
    parser.add_argument('-b', type = int, help='Block Size')
    parser.add_argument('-bp', type = int, help='Bit Pool')
    args = parser.parse_args()

    obj_sbc_frame = sbc_frame()
    if args.f:
        obj_sbc_frame.sampling_freq = args.f
    if args.m:
        supported_channel_modes = ["mono", "dual", "stereo", "joint_stereo"]
        if(args.m in supported_channel_modes):
            obj_sbc_frame.channel_mode = args.m
        else:
            print("supported range for -m: ", supported_channel_modes)
            exit()
    if args.s:
        obj_sbc_frame.sub_bands
    if args.b:
        obj_sbc_frame.block_size = args.b
    if args.bp:
        obj_sbc_frame.bitpool = args.bp
    obj_sbc_frame.get_frame_length()
    obj_sbc_frame.get_audio_duration()
    print(vars(obj_sbc_frame))