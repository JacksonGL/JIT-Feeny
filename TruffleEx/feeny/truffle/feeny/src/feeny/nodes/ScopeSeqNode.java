package feeny.nodes;

import com.oracle.truffle.api.frame.FrameDescriptor;
import com.oracle.truffle.api.frame.VirtualFrame;
import com.oracle.truffle.api.nodes.RootNode;
import com.oracle.truffle.api.nodes.Node.Children;

import feeny.Feeny;
import feeny.reader.ScopeStmt;

public class ScopeSeqNode extends RootNode {

    @Child RootNode firstNode;
    @Child RootNode nextNode;

    public ScopeSeqNode(FrameDescriptor frameDescriptor, ScopeStmt a, ScopeStmt b) {
        super(Feeny.class, null, frameDescriptor);
        firstNode = a.toTruffle(frameDescriptor);
        nextNode = b.toTruffle(frameDescriptor);
    }

    @Override
    public Object execute(VirtualFrame frame) {
        // System.err.println("Evaluating " + this.getClass().getName());
        firstNode.execute(frame);
        return nextNode.execute(frame);
    }

}
