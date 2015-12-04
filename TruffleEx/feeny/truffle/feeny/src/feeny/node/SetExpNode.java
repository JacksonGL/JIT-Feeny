package feeny.node;

import com.oracle.truffle.api.frame.FrameDescriptor;
import com.oracle.truffle.api.frame.FrameSlot;
import com.oracle.truffle.api.frame.VirtualFrame;
import com.oracle.truffle.api.nodes.RootNode;

import feeny.Feeny;

public class SetExpNode extends RootNode {
    String name;
    FrameSlot slot;
    @Child RootNode valueNode;

    public SetExpNode(String name, RootNode valueNode, FrameDescriptor fd) {
        super(Feeny.class, null, fd);
        this.valueNode = valueNode;
        this.slot = fd.findFrameSlot(name);
    }

    @Override
    public Object execute(VirtualFrame frame) {
        Object val = valueNode.execute(frame);
        frame.setObject(slot, val);
        return val;
    }

}
