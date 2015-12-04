package feeny.nodes;

import com.oracle.truffle.api.frame.FrameDescriptor;
import com.oracle.truffle.api.frame.FrameSlot;
import com.oracle.truffle.api.frame.VirtualFrame;
import com.oracle.truffle.api.nodes.RootNode;

import feeny.Feeny;

public class RefExpNode extends RootNode {
    String name;
    FrameSlot slot;

    public RefExpNode(String name, FrameDescriptor fd) {
        super(Feeny.class, null, fd);
        this.name = name;
        this.slot = fd.findFrameSlot(name);
    }

    @Override
    public Object execute(VirtualFrame frame) {
        return frame.getValue(slot);
    }

}
