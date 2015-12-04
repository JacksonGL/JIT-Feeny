package feeny.nodes;

import com.oracle.truffle.api.Truffle;
import com.oracle.truffle.api.frame.FrameDescriptor;
import com.oracle.truffle.api.frame.FrameSlot;
import com.oracle.truffle.api.frame.VirtualFrame;
import com.oracle.truffle.api.nodes.RootNode;

import feeny.Feeny;

public class ScopeVarNode extends RootNode {
    @Child RootNode valueNode;
    String name;
    FrameSlot slot;

    public ScopeVarNode(String name, RootNode rootNode, FrameDescriptor fd) {
        super(Feeny.class, null, fd);
        this.valueNode = rootNode;
        this.name = name;
        this.slot = fd.findFrameSlot(name);
    }

    @Override
    public Object execute(VirtualFrame frame) {
        frame.setObject(slot, valueNode.execute(frame));
        return null;
    }

}
