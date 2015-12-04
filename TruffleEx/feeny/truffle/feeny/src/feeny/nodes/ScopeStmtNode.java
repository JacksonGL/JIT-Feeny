package feeny.nodes;

import com.oracle.truffle.api.frame.FrameDescriptor;
import com.oracle.truffle.api.frame.VirtualFrame;
import com.oracle.truffle.api.nodes.RootNode;

import feeny.Feeny;

public class ScopeStmtNode extends RootNode {

    public ScopeStmtNode(FrameDescriptor frameDescriptor) {
        super(Feeny.class, null, frameDescriptor);
    }

    @Override
    public Object execute(VirtualFrame frame) {

        return null;
    }

}
