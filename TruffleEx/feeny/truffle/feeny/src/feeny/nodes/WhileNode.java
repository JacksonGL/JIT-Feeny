package feeny.nodes;

import com.oracle.truffle.api.RootCallTarget;
import com.oracle.truffle.api.Truffle;
import com.oracle.truffle.api.frame.FrameDescriptor;
import com.oracle.truffle.api.frame.VirtualFrame;
import com.oracle.truffle.api.nodes.RootNode;

import feeny.Feeny;

public class WhileNode extends RootNode {
    @Child RootNode pred_n;
    @Child RootNode body_n;

    public WhileNode(RootNode pred_, RootNode then_, FrameDescriptor frameDescriptor) {
        super(Feeny.class, null, frameDescriptor);
        this.pred_n = pred_;
        this.body_n = then_;
    }

    @Override
    public Object execute(VirtualFrame frame) {
        System.err.println("Evaluating " + this.getClass().getName());
        Object result = null;
        while (pred_n.execute(frame) != null) {
            result = body_n.execute(frame);
        }
        return result;
    }
}
