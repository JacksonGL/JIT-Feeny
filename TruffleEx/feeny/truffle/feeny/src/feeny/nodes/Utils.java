package feeny.nodes;

import com.oracle.truffle.api.frame.MaterializedFrame;
import com.oracle.truffle.api.frame.VirtualFrame;

public class Utils {
    static MaterializedFrame topFrame;

    public static MaterializedFrame getTopFrame(VirtualFrame vf) {
        if (topFrame == null) {
            topFrame = vf.materialize();
        }
        return topFrame;
    }
}
