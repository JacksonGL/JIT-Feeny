package feeny.nodes;

import com.oracle.truffle.api.frame.FrameDescriptor;
import com.oracle.truffle.api.frame.VirtualFrame;
import com.oracle.truffle.api.nodes.RootNode;
import com.oracle.truffle.api.nodes.Node.Children;

import feeny.Feeny;

public class ScopeSeqNode extends RootNode {

    @Children ScopeStmtNode firstNode;
    @Children ScopeStmtNode nextNode;

    public ScopeSeqNode(FrameDescriptor frameDescriptor) {
        super(Feeny.class, null, frameDescriptor);
    }

    @Override
    public Object execute(VirtualFrame frame) {
        firstNode.execute(frame);
        return nextNode.execute(frame);
    }

}
