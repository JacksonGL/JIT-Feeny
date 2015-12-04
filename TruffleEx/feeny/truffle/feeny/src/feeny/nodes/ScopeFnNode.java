package feeny.nodes;

import com.oracle.truffle.api.RootCallTarget;
import com.oracle.truffle.api.Truffle;
import com.oracle.truffle.api.frame.FrameDescriptor;
import com.oracle.truffle.api.frame.FrameSlot;
import com.oracle.truffle.api.frame.FrameSlotKind;
import com.oracle.truffle.api.frame.FrameSlotTypeException;
import com.oracle.truffle.api.frame.VirtualFrame;
import com.oracle.truffle.api.nodes.RootNode;

import feeny.Feeny;
import feeny.reader.ScopeStmt;

public class ScopeFnNode extends RootNode {
    final String args[];
    @Child RootNode body;
    final String name;
    final FrameSlot slot;

    public ScopeFnNode(String name, String[] args2, ScopeStmt body2, FrameDescriptor frameDescriptor) {
        super(Feeny.class, null, frameDescriptor);
        this.args = args2;
        FrameDescriptor fd2 = new FrameDescriptor(frameDescriptor);
        for (int i = 0; i < args.length; i++) {
            fd2.addFrameSlot(args[i]);
        }
        this.body = new FunctionHeaderNode(args2, body2.toTruffle(fd2), fd2);
        this.name = name;
        slot = frameDescriptor.findOrAddFrameSlot(name, FrameSlotKind.Object);
    }

    @Override
    public Object execute(VirtualFrame frame) {
        Object target = Truffle.getRuntime().createCallTarget(body);
        System.err.println("Evaluating " + this.getClass().getName() + ":" + name + " going to " + target);
        Utils.getTopFrame(frame).setObject(slot, target);
        try {
            System.err.println("Evaluating " + this.getClass().getName() + ":" + name + " going to " + Utils.getTopFrame(frame).getObject(slot));
        } catch (FrameSlotTypeException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
        return null;
    }
}
