//
//  Shader.fsh
//  playstream
//
//  Created by Eric Wing on 10/10/12.
//  Copyright (c) 2012 PlayControl Software, LLC. All rights reserved.
//

varying lowp vec4 colorVarying;

void main()
{
    gl_FragColor = colorVarying;
}
