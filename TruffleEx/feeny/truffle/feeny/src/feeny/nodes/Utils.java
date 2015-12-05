package feeny.nodes;

import com.oracle.truffle.api.frame.FrameDescriptor;
import com.oracle.truffle.api.frame.MaterializedFrame;
import com.oracle.truffle.api.frame.VirtualFrame;

public class Utils {
    static MaterializedFrame topFrame;
    static FrameDescriptor topFd;

    public static FrameDescriptor getTopFrameDescriptor(FrameDescriptor fd) {
        if (topFd == null) {
            topFd = fd;
        }
        return topFd;
    }

    public static MaterializedFrame getTopFrame(VirtualFrame vf) {
        if (topFrame == null) {
            topFrame = vf.materialize();
        }
        return topFrame;
    }
}
