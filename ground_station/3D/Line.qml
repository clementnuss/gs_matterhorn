import Qt3D.Core 2.9
import Qt3D.Render 2.9
import Qt3D.Extras 2.9
import QtQuick 2.9

import ch.epfl.ert.groundstation 1.0


Entity{
    objectName:"Line"

    property TraceData traceData


    Material{
        id: lineMaterial
        effect: LineEffect{}
    }

    CuboidMesh{
        id: mesh
    }

    GeometryRenderer{
        id: geometryRenderer

        instanceCount: traceData.count
        primitiveType: GeometryRenderer.LineStrip

        geometry: Geometry{

            Attribute{
                name: "vertexPosition"
                attributeType: Attribute.VertexAttribute
                vertexBaseType: Attribute.Float
                vertexSize: 3
                byteOffset: 0
                byteStride: 3 * 4
                count: traceData.count
                buffer : traceData.buffer
            }
        }
    }

    components: [lineMaterial, geometryRenderer, renderSettings.activeFrameGraph.terrainLayer]
}