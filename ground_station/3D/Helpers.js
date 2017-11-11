
function billboardMV(position, viewMatrix){
    var transform = Qt.matrix4x4();

    transform.m11 = viewMatrix.m11;
    transform.m21 = viewMatrix.m12;
    transform.m31 = viewMatrix.m13;
    transform.m41 = 0;

    transform.m12 = viewMatrix.m21;
    transform.m22 = viewMatrix.m22;
    transform.m32 = viewMatrix.m23;
    transform.m42 = 0;

    transform.m13 = viewMatrix.m31;
    transform.m23 = viewMatrix.m32;
    transform.m33 = viewMatrix.m33;
    transform.m43 = 0;

    transform.m14 = position.x;
    transform.m24 = position.y;
    transform.m34 = position.z;
    transform.m44 = 1;

    return transform
}


function billboard(position, cameraPos, cameraUp) {
            var look = cameraPos.minus(position).normalized();
            var right = cameraUp.crossProduct(look);
            var up2 = look.crossProduct(right);
            var transform = Qt.matrix4x4();

            transform.m11 = right.x;
            transform.m12 = right.y;
            transform.m13 = right.z;
            transform.m14 = 0;

            transform.m21 = up2.x;
            transform.m22 = up2.y;
            transform.m23 = up2.z;
            transform.m24 = 0;

            transform.m31 = look.x;
            transform.m32 = look.y;
            transform.m33 = look.z;
            transform.m34 = 0;

            transform.m41 = position.x;
            transform.m42 = position.y;
            transform.m43 = position.z;
            transform.m44 = 1;

            return transform.transposed()
}