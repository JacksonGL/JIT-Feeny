package feeny.nodes;

import com.oracle.truffle.api.frame.FrameDescriptor;
import com.oracle.truffle.api.frame.FrameSlot;
import com.oracle.truffle.api.frame.VirtualFrame;
import com.oracle.truffle.api.nodes.RootNode;

import feeny.Feeny;

public class RefExpNode extends RootNode {
    final String name;
    final FrameSlot slot;

    public RefExpNode(String name, FrameDescriptor fd) {
        super(Feeny.class, null, fd);
        this.name = name;
        this.slot = fd.findFrameSlot(name);
    }

    @Override
    public Object execute(VirtualFrame frame) {
        System.err.println("Evaluating " + this.getClass().getName());
        return frame.getValue(slot);
    }

}
